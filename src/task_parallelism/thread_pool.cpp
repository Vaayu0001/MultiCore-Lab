#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
using namespace std;

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

        for (thread& worker : workers)
            worker.join();
    }
};

int main()
{
    int results[8] = {0};

    {
        ThreadPool pool(3);

        pool.addTask([&]() { results[0] = 10 + 20; });
        pool.addTask([&]() { results[1] = 5 * 5; });
        pool.addTask([&]() { results[2] = 100 - 40; });
        pool.addTask([&]() { results[3] = 12 * 2; });
        pool.addTask([&]() { results[4] = 50 + 50; });
        pool.addTask([&]() { results[5] = 80 / 2; });
        pool.addTask([&]() { results[6] = 7 * 7; });
        pool.addTask([&]() { results[7] = 200 - 100; });
    }

    cout << "\n--- Thread Pool ---\n";

    for (int i = 0; i < 8; i++)
        cout << "Task " << i + 1 << ": " << results[i] << endl;

    cout << "\nAll Tasks Completed!\n";

    return 0;
}