#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <fstream>
using namespace std;

const int ARRAY_SIZE = 10000000;
const int WARMUP_RUNS = 2;
const int BENCHMARK_RUNS = 15;

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

int main()
{
    vector<int> arr(ARRAY_SIZE);

    for (int i = 0; i < ARRAY_SIZE; i++)
        arr[i] = i % 100;

    const long long expectedResult = sequentialSum(arr);
    int threadCounts[] = {1, 2, 4, 8};

    // Create CSV file
    ofstream csvFile("results/data_parallelism.csv");

    if (!csvFile.is_open())
    {
        cout << "Could not create results/data_parallelism.csv\n";
        return 1;
    }

    csvFile << "array_size,warmup_runs,benchmark_runs,threads,"
            << "sequential_ms,parallel_ms,speedup,efficiency,correctness\n";

    cout << "\n--- Data Parallelism Benchmark ---\n";
    cout << "Array Size: " << ARRAY_SIZE << endl;
    cout << "Warm-up Runs: " << WARMUP_RUNS << endl;
    cout << "Benchmark Runs: " << BENCHMARK_RUNS << endl;

    cout << fixed << setprecision(2);

    cout << "\nThreads\tSequential(ms)\tParallel(ms)"
         << "\tSpeedup\tEfficiency\tCheck\n";

    for (int numThreads : threadCounts)
    {
        // Warm-up
        for (int i = 0; i < WARMUP_RUNS; i++)
        {
            sequentialSum(arr);

            if (parallelSum(arr, 1) != expectedResult)
            {
                cout << "Single-worker verification failed.\n";
                return 1;
            }

            if (parallelSum(arr, numThreads) != expectedResult)
            {
                cout << "Correctness verification failed.\n";
                return 1;
            }
        }

        vector<double> sequentialTimes;
        vector<double> singleWorkerTimes;
        vector<double> parallelTimes;

        bool correct = true;

        // Benchmark
        for (int run = 0; run < BENCHMARK_RUNS; run++)
        {
            long long seqResult = 0;
            long long singleWorkerResult = 0;
            long long parResult = 0;

            auto runSequential = [&]()
            {
                seqResult = sequentialSum(arr);
            };

            auto runParallel = [&]()
            {
                parResult = parallelSum(arr, numThreads);
            };

            auto runSingleWorker = [&]()
            {
                singleWorkerResult = parallelSum(arr, 1);
            };

            // Alternate execution order
            if (run % 2 == 0)
            {
                sequentialTimes.push_back(
                    measureTime(runSequential));

                singleWorkerTimes.push_back(
                    measureTime(runSingleWorker));

                parallelTimes.push_back(
                    measureTime(runParallel));
            }
            else
            {
                parallelTimes.push_back(
                    measureTime(runParallel));

                singleWorkerTimes.push_back(
                    measureTime(runSingleWorker));

                sequentialTimes.push_back(
                    measureTime(runSequential));
            }

            if (seqResult != expectedResult ||
                singleWorkerResult != expectedResult ||
                parResult != expectedResult)
            {
                correct = false;
            }
        }

        if (!correct)
        {
            cout << numThreads
                 << " threads: FAILED verification.\n";
            return 1;
        }

        // Calculate median timings
        double seqTime = medianTime(sequentialTimes);
        double singleWorkerTime = medianTime(singleWorkerTimes);
        double parTime = medianTime(parallelTimes);

        // Speedup compared with sequential execution
        double speedup = seqTime / parTime;

        // Efficiency compared with one parallel worker
        double parallelSpeedup = singleWorkerTime / parTime;
        double efficiency = parallelSpeedup / numThreads * 100.0;

        cout << numThreads << "\t"
             << seqTime << "\t\t"
             << parTime << "\t\t"
             << speedup << "x\t"
             << efficiency << "%\t"
             << "PASSED\n";

        // Save benchmark result to CSV
        csvFile << ARRAY_SIZE << ","
                << WARMUP_RUNS << ","
                << BENCHMARK_RUNS << ","
                << numThreads << ","
                << setprecision(6) << seqTime << ","
                << parTime << ","
                << speedup << ","
                << efficiency << ","
                << "PASSED\n";
    }

    csvFile.close();

    cout << "\nResults saved to results/data_parallelism.csv\n";
    cout << "\nSequential Result Verification: PASSED\n";

    return 0;
}