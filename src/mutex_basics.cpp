#include <iostream>
#include <thread>
#include <mutex>
using namespace std;

int totalCount = 0;
mutex m;

void increment()
{
    for (int i = 0; i < 1000; i++)
    {
        lock_guard<mutex> lock(m);
        totalCount++;
    }
}

int main()
{
    thread t1(increment);
    thread t2(increment);

    t1.join();
    t2.join();

    cout << "Final Count: " << totalCount << endl;

    return 0;
}