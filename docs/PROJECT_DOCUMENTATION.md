MultiCore Lab

Technical Project Documentation

1. Introduction

MultiCore Lab is a project that demonstrates the practical implementation of data parallelism and task parallelism using a multicore processing approach.

The project uses C++ multithreading to execute operations concurrently and compares their performance with sequential execution.

A Flask-based web dashboard is also developed to visualize execution time, speedup and efficiency.

2. System Architecture

The project consists of three major components:

1. C++ Application

* Implements multithreading.
* Executes data parallelism and task parallelism.
* Provides a central menu to access different programs.

2. Benchmark System

* Measures sequential and parallel execution time.
* Calculates speedup and efficiency.
* Stores benchmark results in CSV files.

3. Performance Dashboard

* Uses Python Flask as the backend.
* Reads benchmark data from CSV files.
* Uses HTML, CSS, JavaScript and Chart.js to display results.

3. Data Parallelism Implementation

Data parallelism divides a large data-processing operation into smaller portions. Each portion is processed concurrently by a separate thread.

In this project, data parallelism is demonstrated using an array of 10,000,000 elements.

Each element is initialized using:

arr[i] = i % 100

The program calculates the sum of the array using:

* Sequential execution.
* Parallel execution using multiple threads.

The array is divided into portions, and each thread calculates the sum of its assigned portion. The partial sums are combined to obtain the final result.

Verified result:

* Sequential Sum: 495,000,000
* Parallel Sum: 495,000,000
* Result Verification: PASSED

4. Task Parallelism Implementation

Task parallelism executes different independent operations concurrently.

The basic task parallelism program performs the following operations on an array:

* Calculate the sum.
* Calculate the factorial of a number.
* Find the largest number.
* Count even numbers.

Verified results:

* Sum: 140
* Factorial: 120
* Largest Number: 40
* Even Numbers: 4

Each operation represents an independent task.

5. Thread Pool Implementation

A thread pool is a collection of worker threads that execute tasks from a shared task queue.

Instead of creating a new thread for every task, the thread pool reuses existing worker threads.

In this project:

* 3 worker threads are created.
* 8 tasks are submitted.
* Workers retrieve and execute tasks from the queue.
* The program waits for all tasks to complete.

This approach demonstrates efficient thread management and task execution.

6. Benchmark Methodology

The project measures the performance of sequential and parallel implementations.

The benchmark programs use warm-up runs and repeated measurements to reduce the influence of initial execution overhead and timing fluctuations.

The median execution time is used for comparison.

Data Parallelism Benchmark:

* Array size: 10,000,000
* Warm-up runs: 2
* Benchmark runs: 15
* Thread counts: 1, 2, 4 and 8

Task Parallelism Benchmark:

* Number of tasks: 8
* Separate threads: 8
* Thread pool workers: 3
* Warm-up runs: 2
* Benchmark runs: 15

7. Performance Metrics

The project uses three main performance metrics.

Execution Time

The time required by a program to complete an operation.

Speedup

Speedup measures how much faster parallel execution is compared with sequential execution.

Formula:

Speedup = Sequential Execution Time / Parallel Execution Time

Efficiency

Efficiency measures how effectively multiple workers are utilized.

Formula:

Efficiency = (Speedup / Number of Workers) × 100

For the benchmark programs, efficiency is calculated relative to the corresponding single-worker parallel baseline.

8. Data Parallelism Benchmark Results

The following results were obtained during the verified benchmark run.

Threads: 1
Sequential Time: 48.82 ms
Parallel Time: 29.15 ms
Speedup: 1.67x
Efficiency: 99.93%

Threads: 2
Sequential Time: 47.71 ms
Parallel Time: 16.95 ms
Speedup: 2.81x
Efficiency: 87.18%

Threads: 4
Sequential Time: 47.79 ms
Parallel Time: 9.74 ms
Speedup: 4.90x
Efficiency: 76.21%

Threads: 8
Sequential Time: 47.02 ms
Parallel Time: 9.92 ms
Speedup: 4.74x
Efficiency: 37.25%

Analysis:

Four threads achieved the highest measured speedup of 4.90x.

Increasing the thread count from four to eight did not improve performance in this benchmark. This demonstrates that additional threads do not always result in better performance because of thread management overhead and hardware resource limitations.

9. Task Parallelism Benchmark Results

Method: Sequential
Workers: 1
Execution Time: 75.22 ms
Speedup: 1.00x
Efficiency: N/A

Method: Separate Threads
Workers: 8
Execution Time: 16.37 ms
Speedup: 4.60x
Efficiency: 57.50%

Method: Thread Pool
Workers: 3
Execution Time: 29.94 ms
Speedup: 2.51x
Efficiency: 83.81%

Analysis:

Separate threads achieved a higher speedup in the measured benchmark.

The thread pool achieved higher efficiency relative to its single-worker thread-pool baseline.

The results demonstrate the performance differences between creating separate threads and reusing worker threads through a thread pool.

10. Dashboard Implementation

The project includes a web-based performance dashboard developed using Flask and Chart.js.

Backend:

* Python Flask.
* Reads CSV benchmark files.
* Provides benchmark data through the /api/data endpoint.
* Serves the dashboard homepage.

Frontend:

* HTML for page structure.
* CSS for styling.
* JavaScript for data handling.
* Chart.js for graphical visualization.

Dashboard features:

* Dashboard overview.
* Data parallelism analysis.
* Task parallelism analysis.
* Execution time comparison.
* Speedup comparison.
* Efficiency comparison.
* Benchmark results table.

The dashboard reads the latest results from:

* results/data_parallelism.csv
* results/task_parallelism.csv

11. Testing and Verification

The following tests were completed successfully:

1. All nine CMake build targets compiled successfully.
2. Basic data parallelism produced the correct sum.
3. Data parallelism benchmark passed correctness checks.
4. Basic task parallelism produced the expected results.
5. All eight thread pool tasks completed successfully.
6. Task parallelism benchmark passed correctness checks.
7. Both CSV benchmark files were generated successfully.
8. Flask dashboard loaded successfully.
9. Dashboard API returned HTTP 200.
10. Dashboard displayed the latest benchmark results.

12. Conclusion

MultiCore Lab successfully demonstrates the implementation of data parallelism and task parallelism using C++ multithreading.

The project compares sequential execution, separate threads and thread pool execution through practical benchmark measurements.

The Flask-based dashboard provides a graphical representation of performance results, making it easier to understand the relationship between execution time, speedup and efficiency.

The project provides practical understanding of concurrent programming, thread management, parallel execution and performance analysis.