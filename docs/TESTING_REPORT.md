MULTICORE LAB

TESTING REPORT

1. Introduction

This document describes the testing and verification performed on the MultiCore Lab project.

The purpose of testing was to verify that all programs execute correctly, benchmark results are generated successfully, and the performance dashboard displays the correct data.

2. Compilation Testing

The complete project was compiled using CMake.

Command:

cmake –build build

Result:

All 9 build targets compiled successfully without errors.

Status: PASSED

3. Central Application Testing

The central application was executed using:

./build/multicore_lab

The following menu options were tested:

1. Basic Data Parallelism — PASSED
2. Data Parallelism Benchmark — PASSED
3. Basic Task Parallelism — PASSED
4. Thread Pool Demonstration — PASSED
5. Task Parallelism Benchmark — PASSED
6. Exit — PASSED

Result:

All menu options executed successfully.

4. Data Parallelism Testing

The program processed an array containing 10,000,000 elements.

Expected Sum: 495000000

Sequential Sum: 495000000

Parallel Sum: 495000000

Result Verification: PASSED

The benchmark was tested with 1, 2, 4 and 8 threads.

All benchmark correctness checks passed.

5. Task Parallelism Testing

The basic task parallelism program was tested with the following operations:

Sum: 140

Factorial: 120

Largest Number: 40

Even Numbers: 4

Result: PASSED

6. Thread Pool Testing

The thread pool was tested using 3 worker threads and 8 independent tasks.

All 8 tasks completed successfully and produced the expected results.

Result: PASSED

7. Benchmark Testing

Both benchmark programs were executed successfully.

Data Parallelism Benchmark:

* 2 warm-up runs.
* 15 benchmark runs.
* Thread counts: 1, 2, 4 and 8.
* All correctness checks passed.

Task Parallelism Benchmark:

* 2 warm-up runs.
* 15 benchmark runs.
* Compared sequential execution, separate threads and thread pool execution.
* Correctness verification passed.

8. CSV File Testing

The generated CSV files were checked.

Files:

results/data_parallelism.csv

results/task_parallelism.csv

Both files contained the expected headers and benchmark records.

Result: PASSED

9. Dashboard Testing

The Flask dashboard was tested through a web browser.

The following components were verified:

* Dashboard homepage.
* Data Parallelism charts.
* Task Parallelism charts.
* Execution time comparison.
* Speedup comparison.
* Efficiency comparison.
* Benchmark results table.

The Flask homepage and benchmark API both returned HTTP 200.

The dashboard displayed the latest benchmark results from the CSV files.

Result: PASSED

10. Final Testing Summary

Compilation Testing: PASSED

Central Application Testing: PASSED

Data Parallelism Testing: PASSED

Task Parallelism Testing: PASSED

Thread Pool Testing: PASSED

Benchmark Testing: PASSED

CSV File Testing: PASSED

Dashboard Testing: PASSED

11. Conclusion

The MultiCore Lab project passed the performed compilation, functional, benchmark and dashboard integration tests.

All implemented programs executed successfully, benchmark results were stored in CSV files, and the web dashboard displayed the generated performance data correctly.
