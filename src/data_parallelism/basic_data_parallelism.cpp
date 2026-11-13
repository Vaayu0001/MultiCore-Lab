#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
using namespace std;

const int ARRAY_SIZE = 10000000;
const int NUM_THREADS = 4;

long long sequentialSum(const vector<int>& arr)
{
    long long sum = 0;

    for (int num : arr)
        sum += num;

    return sum;
}

void parallelSum(const vector<int>& arr, long long partialSums[], int id)
{
    int start = id * ARRAY_SIZE / NUM_THREADS;
    int end = (id + 1) * ARRAY_SIZE / NUM_THREADS;

    for (int i = start; i < end; i++)
        partialSums[id] += arr[i];
}

int main()
{
    vector<int> arr(ARRAY_SIZE);

    for (int i = 0; i < ARRAY_SIZE; i++)
        arr[i] = i % 100;

    // Sequential execution
    auto start = chrono::steady_clock::now();

    long long seqResult = sequentialSum(arr);

    auto end = chrono::steady_clock::now();

    double seqTime =
        chrono::duration<double, milli>(end - start).count();

    // Parallel execution
    long long partialSums[NUM_THREADS] = {0};
    thread workers[NUM_THREADS];

    start = chrono::steady_clock::now();

    for (int i = 0; i < NUM_THREADS; i++)
        workers[i] = thread(parallelSum, ref(arr), partialSums, i);

    for (int i = 0; i < NUM_THREADS; i++)
        workers[i].join();

    long long parResult = 0;

    for (int i = 0; i < NUM_THREADS; i++)
        parResult += partialSums[i];

    end = chrono::steady_clock::now();

    double parTime =
        chrono::duration<double, milli>(end - start).count();

    // Results
    cout << "\n--- MultiCore Lab ---\n";

    cout << "Sequential Sum: " << seqResult << endl;
    cout << "Parallel Sum:   " << parResult << endl;

    cout << "\nSequential Time: " << seqTime << " ms" << endl;
    cout << "Parallel Time:   " << parTime << " ms" << endl;

    cout << "\nSpeedup: " << seqTime / parTime << "x" << endl;

    cout << "Efficiency: "
         << (seqTime / parTime) / NUM_THREADS * 100
         << "%" << endl;

    if (seqResult == parResult)
        cout << "\nResult Verification: PASSED" << endl;
    else
        cout << "\nResult Verification: FAILED" << endl;

    return 0;
}