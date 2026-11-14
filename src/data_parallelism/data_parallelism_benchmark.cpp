#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>
#include <stdexcept>

using namespace std;

long long sequentialSum(const vector<int>& arr)
{
    long long sum = 0;

    for (int num : arr)
        sum += num;

    return sum;
}

long long parallelSum(const vector<int>& arr, int numThreads)
{
    vector<long long> partialSums(numThreads, 0);
    vector<thread> workers;

    for (int id = 0; id < numThreads; id++)
    {
        workers.emplace_back([&, id]()
        {
            size_t start = arr.size() * id / numThreads;
            size_t end = arr.size() * (id + 1) / numThreads;

            for (size_t i = start; i < end; i++)
                partialSums[id] += arr[i];
        });
    }

    for (auto& worker : workers)
        worker.join();

    long long sum = 0;

    for (long long part : partialSums)
        sum += part;

    return sum;
}

template <typename Function>
double measureTime(Function function)
{
    auto start = chrono::steady_clock::now();

    function();

    auto end = chrono::steady_clock::now();

    return chrono::duration<double, milli>(end - start).count();
}

double medianTime(vector<double> times)
{
    sort(times.begin(), times.end());

    return times[times.size() / 2];
}

int readNumber(const string& value)
{
    size_t position = 0;
    int number = stoi(value, &position);

    if (position != value.size())
        throw invalid_argument("Invalid number.");

    return number;
}

vector<int> readThreads(const string& input)
{
    vector<int> threads;
    stringstream ss(input);
    string value;

    while (getline(ss, value, ','))
    {
        int count = readNumber(value);

        if (count < 1 || count > 256)
            throw invalid_argument("Workers must be between 1 and 256.");

        if (find(threads.begin(), threads.end(), count) != threads.end())
            throw invalid_argument("Duplicate worker count.");

        threads.push_back(count);
    }

    if (threads.empty())
        throw invalid_argument("At least one worker count is required.");

    return threads;
}

int main(int argc, char* argv[])
{
    int arraySize = 10000000;
    int warmupRuns = 2;
    int benchmarkRuns = 15;

    vector<int> threadCounts = {1, 2, 4, 8};

    try
    {
        if (argc != 1 && argc != 5)
        {
            throw invalid_argument(
                "Usage: benchmark [array_size warmup_runs benchmark_runs workers]");
        }

        if (argc == 5)
        {
            arraySize = readNumber(argv[1]);
            warmupRuns = readNumber(argv[2]);
            benchmarkRuns = readNumber(argv[3]);
            threadCounts = readThreads(argv[4]);
        }

        if (arraySize < 1000 || arraySize > 50000000)
            throw invalid_argument("Array size must be between 1000 and 50000000.");

        if (warmupRuns < 0 || warmupRuns > 20)
            throw invalid_argument("Warm-up runs must be between 0 and 20.");

        if (benchmarkRuns < 1 || benchmarkRuns > 100)
            throw invalid_argument("Benchmark runs must be between 1 and 100.");

        vector<int> arr(arraySize);

        for (int i = 0; i < arraySize; i++)
            arr[i] = i % 100;

        const long long expectedResult = sequentialSum(arr);

        cout << fixed << setprecision(6);

        cout << "{";
        cout << "\"benchmark\":\"data_parallelism\",";
        cout << "\"array_size\":" << arraySize << ",";
        cout << "\"warmup_runs\":" << warmupRuns << ",";
        cout << "\"benchmark_runs\":" << benchmarkRuns << ",";
        cout << "\"results\":[";

        bool first = true;

        for (int numThreads : threadCounts)
        {
            for (int i = 0; i < warmupRuns; i++)
            {
                if (parallelSum(arr, numThreads) != expectedResult)
                    throw runtime_error("Warm-up correctness verification failed.");
            }

            vector<double> sequentialTimes;
            vector<double> singleWorkerTimes;
            vector<double> parallelTimes;

            bool correct = true;

            for (int run = 0; run < benchmarkRuns; run++)
            {
                long long seqResult = 0;
                long long singleResult = 0;
                long long parallelResult = 0;

                auto runSequential = [&]()
                {
                    seqResult = sequentialSum(arr);
                };

                auto runSingle = [&]()
                {
                    singleResult = parallelSum(arr, 1);
                };

                auto runParallel = [&]()
                {
                    parallelResult = parallelSum(arr, numThreads);
                };

                if (run % 2 == 0)
                {
                    sequentialTimes.push_back(measureTime(runSequential));
                    singleWorkerTimes.push_back(measureTime(runSingle));
                    parallelTimes.push_back(measureTime(runParallel));
                }
                else
                {
                    parallelTimes.push_back(measureTime(runParallel));
                    singleWorkerTimes.push_back(measureTime(runSingle));
                    sequentialTimes.push_back(measureTime(runSequential));
                }

                if (seqResult != expectedResult ||
                    singleResult != expectedResult ||
                    parallelResult != expectedResult)
                {
                    correct = false;
                }
            }

            if (!correct)
                throw runtime_error("Correctness verification failed.");

            double seqTime = medianTime(sequentialTimes);
            double singleTime = medianTime(singleWorkerTimes);
            double parallelTime = medianTime(parallelTimes);

            double speedup = seqTime / parallelTime;

            double efficiency =
                (singleTime / parallelTime) / numThreads * 100.0;

            if (!first)
                cout << ",";

            first = false;

            cout << "{";
            cout << "\"threads\":" << numThreads << ",";
            cout << "\"sequential_ms\":" << seqTime << ",";
            cout << "\"parallel_ms\":" << parallelTime << ",";
            cout << "\"speedup\":" << speedup << ",";
            cout << "\"efficiency\":" << efficiency << ",";
            cout << "\"correctness\":\"PASSED\"";
            cout << "}";
        }

        cout << "]}";
        cout << endl;

        return 0;
    }
    catch (const exception& error)
    {
        cerr << "Benchmark error: " << error.what() << endl;
        return 1;
    }
}