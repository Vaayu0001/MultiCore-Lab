#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>
#include <stdexcept>

using namespace std;

using ull = unsigned long long;

int TASKS = 8;
int WORK = 2000000;
int WARMUP_RUNS = 2;
int RUNS = 15;

vector<int> workerCounts = {1, 3};

ull calculateTask(int id)
{
    ull result = id + 1;

    for (int i = 0; i < WORK; i++)
        result = (result * 1664525ULL + 1013904223ULL)
                 % 4294967296ULL;

    return result;
}

class ThreadPool
{
    vector<thread> workers;
    queue<function<void()>> tasks;
    mutex m;
    condition_variable cv;
    bool stop = false;

public:
    ThreadPool(int n)
    {
        for (int i = 0; i < n; i++)
        {
            workers.emplace_back([this]()
            {
                while (true)
                {
                    function<void()> task;

                    {
                        unique_lock<mutex> lock(m);

                        cv.wait(lock, [this]()
                        {
                            return stop || !tasks.empty();
                        });

                        if (stop && tasks.empty())
                            return;

                        task = tasks.front();
                        tasks.pop();
                    }

                    task();
                }
            });
        }
    }

    void addTask(function<void()> task)
    {
        {
            lock_guard<mutex> lock(m);
            tasks.push(task);
        }

        cv.notify_one();
    }

    ~ThreadPool()
    {
        {
            lock_guard<mutex> lock(m);
            stop = true;
        }

        cv.notify_all();

        for (auto& worker : workers)
            worker.join();
    }
};

vector<ull> sequentialExecution()
{
    vector<ull> results(TASKS);

    for (int i = 0; i < TASKS; i++)
        results[i] = calculateTask(i);

    return results;
}

vector<ull> singleThreadExecution()
{
    vector<ull> results(TASKS);

    thread worker([&]()
    {
        for (int i = 0; i < TASKS; i++)
            results[i] = calculateTask(i);
    });

    worker.join();

    return results;
}

vector<ull> separateThreads()
{
    vector<ull> results(TASKS);
    vector<thread> workers;

    for (int i = 0; i < TASKS; i++)
    {
        workers.emplace_back([&, i]()
        {
            results[i] = calculateTask(i);
        });
    }

    for (auto& worker : workers)
        worker.join();

    return results;
}

vector<ull> poolExecution(int workerCount)
{
    vector<ull> results(TASKS);

    {
        ThreadPool pool(workerCount);

        for (int i = 0; i < TASKS; i++)
        {
            pool.addTask([&, i]()
            {
                results[i] = calculateTask(i);
            });
        }
    }

    return results;
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

int readNumber(string value, string name)
{
    size_t position = 0;
    int number = stoi(value, &position);

    if (position != value.size())
        throw invalid_argument("Invalid " + name);

    return number;
}

vector<int> readWorkers(string input)
{
    vector<int> workers;
    stringstream stream(input);
    string value;

    while (getline(stream, value, ','))
    {
        if (value.empty())
            throw invalid_argument("Invalid worker list");

        int n = readNumber(value, "worker count");

        if (n < 1 || n > TASKS)
            throw invalid_argument("Workers must be between 1 and number of tasks");

        if (find(workers.begin(), workers.end(), n) != workers.end())
            throw invalid_argument("Duplicate worker count");

        workers.push_back(n);
    }

    if (workers.empty())
        throw invalid_argument("Worker list cannot be empty");

    return workers;
}

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 1 && argc != 6)
            throw invalid_argument(
                "Usage: program [tasks work warmup_runs benchmark_runs workers]"
            );

        if (argc == 6)
        {
            TASKS = readNumber(argv[1], "task count");
            WORK = readNumber(argv[2], "work per task");
            WARMUP_RUNS = readNumber(argv[3], "warmup runs");
            RUNS = readNumber(argv[4], "benchmark runs");

            if (TASKS < 1 || TASKS > 64)
                throw invalid_argument("Tasks must be between 1 and 64");

            if (WORK < 1000 || WORK > 10000000)
                throw invalid_argument("Work must be between 1000 and 10000000");

            if (1LL * TASKS * WORK > 100000000)
                throw invalid_argument("Total workload cannot exceed 100 million iterations");

            if (WARMUP_RUNS < 0 || WARMUP_RUNS > 20)
                throw invalid_argument("Warmup runs must be between 0 and 20");

            if (RUNS < 1 || RUNS > 100)
                throw invalid_argument("Benchmark runs must be between 1 and 100");

            workerCounts = readWorkers(argv[5]);
        }

        vector<ull> expected = sequentialExecution();

        vector<double> seqTimes;
        vector<double> singleThreadTimes;
        vector<double> threadTimes;
        vector<double> singlePoolTimes;

        vector<vector<double>> poolTimes(workerCounts.size());

        // Warm-up
        for (int i = 0; i < WARMUP_RUNS; i++)
        {
            sequentialExecution();
            singleThreadExecution();
            separateThreads();

            poolExecution(1);

            for (int workers : workerCounts)
                poolExecution(workers);
        }

        // Benchmark
        for (int i = 0; i < RUNS; i++)
        {
            auto runSequential = [&]()
            {
                seqTimes.push_back(measureTime([]()
                {
                    sequentialExecution();
                }));
            };

            auto runSingleThread = [&]()
            {
                singleThreadTimes.push_back(measureTime([]()
                {
                    singleThreadExecution();
                }));
            };

            auto runThreads = [&]()
            {
                threadTimes.push_back(measureTime([]()
                {
                    separateThreads();
                }));
            };

            auto runSinglePool = [&]()
            {
                singlePoolTimes.push_back(measureTime([]()
                {
                    poolExecution(1);
                }));
            };

            auto runPools = [&]()
            {
                for (size_t j = 0; j < workerCounts.size(); j++)
                {
                    int workers = workerCounts[j];

                    poolTimes[j].push_back(measureTime([workers]()
                    {
                        poolExecution(workers);
                    }));
                }
            };

            if (i % 2 == 0)
            {
                runSequential();
                runSingleThread();
                runThreads();
                runSinglePool();
                runPools();
            }
            else
            {
                runPools();
                runSinglePool();
                runThreads();
                runSingleThread();
                runSequential();
            }
        }

        bool correct = (expected == singleThreadExecution() &&
                        expected == separateThreads() &&
                        expected == poolExecution(1));

        for (int workers : workerCounts)
        {
            if (expected != poolExecution(workers))
                correct = false;
        }

        double seq = medianTime(seqTimes);
        double singleThread = medianTime(singleThreadTimes);
        double threads = medianTime(threadTimes);
        double singlePool = medianTime(singlePoolTimes);

        double threadSpeedup = seq / threads;
        double threadEfficiency =
            (singleThread / threads) / TASKS * 100.0;

        cout << fixed << setprecision(6);

        cout << "{";
        cout << "\"benchmark\":\"task_parallelism\",";
        cout << "\"tasks\":" << TASKS << ",";
        cout << "\"work_per_task\":" << WORK << ",";
        cout << "\"warmup_runs\":" << WARMUP_RUNS << ",";
        cout << "\"benchmark_runs\":" << RUNS << ",";
        cout << "\"results\":[";

        cout << "{";
        cout << "\"workers\":1,";
        cout << "\"method\":\"Sequential\",";
        cout << "\"execution_ms\":" << seq << ",";
        cout << "\"speedup\":1.0,";
        cout << "\"efficiency\":null,";
        cout << "\"correctness\":\""
             << (correct ? "PASSED" : "FAILED") << "\"";
        cout << "},";

        cout << "{";
        cout << "\"workers\":" << TASKS << ",";
        cout << "\"method\":\"Separate Threads\",";
        cout << "\"execution_ms\":" << threads << ",";
        cout << "\"speedup\":" << threadSpeedup << ",";
        cout << "\"efficiency\":" << threadEfficiency << ",";
        cout << "\"correctness\":\""
             << (correct ? "PASSED" : "FAILED") << "\"";
        cout << "},";

        for (size_t i = 0; i < workerCounts.size(); i++)
        {
            int workers = workerCounts[i];

            double pool = medianTime(poolTimes[i]);
            double speedup = seq / pool;
            double efficiency =
                (singlePool / pool) / workers * 100.0;

            cout << "{";
            cout << "\"workers\":" << workers << ",";
            cout << "\"method\":\"Thread Pool\",";
            cout << "\"execution_ms\":" << pool << ",";
            cout << "\"speedup\":" << speedup << ",";
            cout << "\"efficiency\":" << efficiency << ",";
            cout << "\"correctness\":\""
                 << (correct ? "PASSED" : "FAILED") << "\"";
            cout << "}";

            if (i + 1 < workerCounts.size())
                cout << ",";
        }

        cout << "]}";

        return correct ? 0 : 1;
    }
    catch (const exception& error)
    {
        cerr << "Error: " << error.what() << endl;
        return 1;
    }
}