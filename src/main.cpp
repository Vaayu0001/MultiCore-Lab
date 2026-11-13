#include <iostream>
#include <thread>
using namespace std;

int main()
{
    thread t([]()
    {
        cout << "Worker thread is running!" << endl;
    });

    cout << "Main thread is running!" << endl;

    t.join();

    cout << "Program finished!" << endl;

    return 0;
}