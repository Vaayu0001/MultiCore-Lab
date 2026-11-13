#include <iostream>
#include <thread>
using namespace std;

void task(int n)
{
    cout << "Thread " << n << " is running" << endl;
}

int main()
{
    thread t1(task, 1);
    thread t2(task, 2);
    thread t3(task, 3);

    t1.join();
    t2.join();
    t3.join();

    cout << "All threads finished!" << endl;

    return 0;
}