#include <iostream>
#include <cstdlib>
using namespace std;

void runProgram(const string& name, const string& command)
{
    cout << "\nRunning " << name << "...\n\n";

    int result = system(command.c_str());

    if (result == 0)
        cout << "\nProgram completed successfully.\n";
    else
        cout << "\nProgram execution failed.\n";
}

int main()
{
    int choice;

    do
    {
        cout << "\n================================\n";
        cout << "          MULTICORE LAB\n";
        cout << "================================\n";

        cout << "1. Basic Data Parallelism\n";
        cout << "2. Data Parallelism Benchmark\n";
        cout << "3. Basic Task Parallelism\n";
        cout << "4. Thread Pool Demonstration\n";
        cout << "5. Task Parallelism Benchmark\n";
        cout << "0. Exit\n";

        cout << "\nEnter your choice: ";
        cin >> choice;

        switch (choice)
        {
            case 1:
                runProgram("Basic Data Parallelism",
                           "./build/basic_data_parallelism");
                break;

            case 2:
                runProgram("Data Parallelism Benchmark",
                           "./build/data_parallelism_benchmark");
                break;

            case 3:
                runProgram("Basic Task Parallelism",
                           "./build/basic_task_parallelism");
                break;

            case 4:
                runProgram("Thread Pool",
                           "./build/thread_pool");
                break;

            case 5:
                runProgram("Task Parallelism Benchmark",
                           "./build/task_parallelism_benchmark");
                break;

            case 0:
                cout << "\nExiting MultiCore Lab. Goodbye!\n";
                break;

            default:
                cout << "\nInvalid choice. Please try again.\n";
        }

    } while (choice != 0);

    return 0;
}