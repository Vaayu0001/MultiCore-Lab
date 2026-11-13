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
#include <fstream>
using namespace std;

const int TASKS = 8;
const int WORKERS = 3;
const int WARMUP_RUNS = 2;
const int RUNS = 15;
const int WORK = 2000000;

using ull = unsigned long long;

ull calculateTask(int id)
{
    ull result = id + 1;

    for (int i = 0; i < WORK; i++)
        result = (result * 1664525ULL + 1013904223ULL) % 4294967296ULL;

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

int main()
{
    vector<ull> expected = sequentialExecution();

    // Open CSV file
    ofstream csvFile("results/task_parallelism.csv");

    if (!csvFile.is_open())
    {
        cout << "Could not create results/task_parallelism.csv\n";
        return 1;
    }

    csvFile << "tasks,workers,method,execution_ms,speedup,efficiency,correctness\n";

    vector<double> seqTimes;
    vector<double> singleThreadTimes;
    vector<double> threadTimes;
    vector<double> singlePoolTimes;
    vector<double> poolTimes;

    // Warm-up
    for (int i = 0; i < WARMUP_RUNS; i++)
    {
        sequentialExecution();
        singleThreadExecution();
        separateThreads();
        poolExecution(1);
        poolExecution(WORKERS);
    }

    // Benchmark
    for (int i = 0; i < RUNS; i++)
    {
        if (i % 2 == 0)
        {
            seqTimes.push_back(measureTime([]()
            {
                sequentialExecution();
            }));

            singleThreadTimes.push_back(measureTime([]()
            {
                singleThreadExecution();
            }));

            threadTimes.push_back(measureTime([]()
            {
                separateThreads();
            }));

            singlePoolTimes.push_back(measureTime([]()
            {
                poolExecution(1);
            }));

            poolTimes.push_back(measureTime([]()
            {
                poolExecution(WORKERS);
            }));
        }
        else
        {
            poolTimes.push_back(measureTime([]()
            {
                poolExecution(WORKERS);
            }));

            singlePoolTimes.push_back(measureTime([]()
            {
                poolExecution(1);
            }));

            threadTimes.push_back(measureTime([]()
            {
                separateThreads();
            }));

            singleThreadTimes.push_back(measureTime([]()
            {
                singleThreadExecution();
            }));

            seqTimes.push_back(measureTime([]()
            {
                sequentialExecution();
            }));
        }
    }

    vector<ull> threadResult = separateThreads();
    vector<ull> singleThreadResult = singleThreadExecution();
    vector<ull> poolResult = poolExecution(WORKERS);
    vector<ull> singlePoolResult = poolExecution(1);

    bool correct = (expected == threadResult &&
                    expected == singleThreadResult &&
                    expected == poolResult &&
                    expected == singlePoolResult);

    double seq = medianTime(seqTimes);
    double singleThread = medianTime(singleThreadTimes);
    double threads = medianTime(threadTimes);
    double singlePool = medianTime(singlePoolTimes);
    double pool = medianTime(poolTimes);

    double threadSpeedup = seq / threads;
    double poolSpeedup = seq / pool;

    double threadEfficiency =
        (singleThread / threads) / TASKS * 100.0;

    double poolEfficiency =
        (singlePool / pool) / WORKERS * 100.0;

    cout << "\n--- Task Parallelism Benchmark ---\n";
    cout << "Number of Tasks: " << TASKS << endl;
    cout << "Separate Threads: " << TASKS << endl;
    cout << "Thread Pool Workers: " << WORKERS << endl;
    cout << "Warm-up Runs: " << WARMUP_RUNS << endl;
    cout << "Benchmark Runs: " << RUNS << endl;

    cout << fixed << setprecision(2);

    cout << "\nMethod\t\tTime(ms)\tSpeedup\tEfficiency\n";

    cout << "Sequential\t" << seq << "\t\t1.00x\t-\n";

    cout << "Threads\t\t" << threads << "\t\t"
         << threadSpeedup << "x\t"
         << threadEfficiency << "%\n";

    cout << "Thread Pool\t" << pool << "\t\t"
         << poolSpeedup << "x\t"
         << poolEfficiency << "%\n";

    cout << "\nCorrectness: "
         << (correct ? "PASSED" : "FAILED") << endl;

    // Save results to CSV
    csvFile << fixed << setprecision(6);

    csvFile << TASKS << ",1,Sequential,"
            << seq << ",1.0,,"
            << (correct ? "PASSED" : "FAILED") << "\n";

    csvFile << TASKS << "," << TASKS << ",Separate Threads,"
            << threads << "," << threadSpeedup << ","
            << threadEfficiency << ","
            << (correct ? "PASSED" : "FAILED") << "\n";

    csvFile << TASKS << "," << WORKERS << ",Thread Pool,"
            << pool << "," << poolSpeedup << ","
            << poolEfficiency << ","
            << (correct ? "PASSED" : "FAILED") << "\n";

    csvFile.close();

    cout << "\nResults saved to results/task_parallelism.csv\n";

    return correct ? 0 : 1;
}