#include <iostream>
#include <vector>
#include <thread>
#include <algorithm>
using namespace std;

// Task 1: Calculate sum
long long calculateSum(const vector<int>& arr)
{
    long long sum = 0;

    for (int num : arr)
        sum += num;

    return sum;
}

// Task 2: Calculate factorial
long long factorial(int n)
{
    long long result = 1;

    for (int i = 1; i <= n; i++)
        result *= i;

    return result;
}

// Task 3: Find largest number
int findLargest(const vector<int>& arr)
{
    return *max_element(arr.begin(), arr.end());
}

// Task 4: Count even numbers
int countEven(const vector<int>& arr)
{
    int count = 0;

    for (int num : arr)
    {
        if (num % 2 == 0)
            count++;
    }

    return count;
}

int main()
{
    vector<int> arr = {10, 20, 15, 30, 25, 40};

    long long sum;
    long long fact;
    int largest;
    int evenCount;

    // Create worker threads
    thread t1([&]() { sum = calculateSum(arr); });
    thread t2([&]() { fact = factorial(5); });
    thread t3([&]() { largest = findLargest(arr); });
    thread t4([&]() { evenCount = countEven(arr); });

    // Wait for all tasks to finish
    t1.join();
    t2.join();
    t3.join();
    t4.join();

    // Display results
    cout << "\n--- Task Parallelism ---\n";

    cout << "Sum: " << sum << endl;
    cout << "Factorial: " << fact << endl;
    cout << "Largest Number: " << largest << endl;
    cout << "Even Numbers: " << evenCount << endl;

    cout << "\nAll Tasks Completed Successfully!\n";

    return 0;
}