MultiCore Lab

Virtual Implementation of Data Parallelism and Task Parallelism with a Multicore Processing Unit

1. Project Overview

MultiCore Lab is a C++17-based project that demonstrates data parallelism and task parallelism using multithreading.

The project compares sequential and parallel execution, measures performance, calculates speedup and efficiency, and presents benchmark results through a web-based dashboard.

2. Objectives

* Understand multithreading and multicore processing.
* Implement data parallelism using C++ threads.
* Implement task parallelism using independent tasks.
* Demonstrate thread pool execution.
* Compare sequential and parallel performance.
* Calculate execution time, speedup and efficiency.
* Visualize benchmark results using a web dashboard.

3. Technologies Used

* C++17
* Standard C++ Thread Library
* CMake
* Python 3
* Flask
* HTML and CSS
* JavaScript
* Chart.js
* CSV

4. Project Modules

Data Parallelism

Divides a large data-processing operation into smaller portions that can execute concurrently.

Includes:

* Basic Data Parallelism
* Data Parallelism Benchmark

Task Parallelism

Executes independent tasks concurrently.

Includes:

* Basic Task Parallelism
* Thread Pool Demonstration
* Task Parallelism Benchmark

Performance Dashboard

Displays benchmark results using charts and tables.

Includes:

* Dashboard Overview
* Data Parallelism Analysis
* Task Parallelism Analysis
* Benchmark Results

5. Project Structure

MultiCore-Lab/
├── src/
│   ├── data_parallelism/
│   ├── task_parallelism/
│   ├── main.cpp
│   ├── main_menu.cpp
│   ├── mutex_basics.cpp
│   └── thread_basics.cpp
├── include/
├── tests/
├── benchmarks/
├── results/
│   ├── data_parallelism.csv
│   └── task_parallelism.csv
├── dashboard/
│   ├── app.py
│   ├── templates/
│   │   └── index.html
│   └── venv/
├── docs/
├── build/
├── CMakeLists.txt
└── README.md

6. Requirements

* C++17-compatible compiler
* CMake 3.20 or later
* Python 3
* Flask

7. Build Instructions

From the project root directory:
cmake -S . -B build
cmake --build build

8. Run the C++ Application

./build/multicore_lab

Main Menu

1. Basic Data Parallelism
2. Data Parallelism Benchmark
3. Basic Task Parallelism
4. Thread Pool Demonstration
5. Task Parallelism Benchmark
6. Exit

9. Run the Performance Dashboard

- Activate the Python virtual environment:

source dashboard/venv/bin/activate

- Start the Flask application:

python dashboard/app.py

- Open the following address in your browser:

http://127.0.0.1:5000/

10. Performance Metrics

The project uses the following metrics:

* Execution Time: Time required to complete an operation.
* Speedup: Ratio of sequential execution time to parallel execution time.
* Efficiency: Measures parallel performance relative to the number of workers, using the relevant single-worker baseline.

Benchmark results are stored in CSV files inside the results/ directory.

11. Conclusion

MultiCore Lab demonstrates how data parallelism and task parallelism can be implemented using multithreading.

It provides practical comparisons between sequential execution, separate threads and thread pool execution, supported by benchmark measurements and a graphical performance dashboard.
