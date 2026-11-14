# MULTICORE LAB — COMPLETE PROJECT CONTEXT

Document Purpose:
This file is the permanent technical handoff and project reference for
MultiCore Lab. It records the project's identity, architecture,
implementation, benchmark methodology, verified results, development
environment, testing status, GitHub configuration, deployment
preparation, known issues, and future work.

Future ChatGPT conversations working on this project should read this
file before suggesting changes or asking the user to explain the project.

Last verified project handoff:
14 November 2026

============================================================
1. PROJECT IDENTITY
============================================================

Project Name:
MultiCore Lab

Full Project Title:
Virtual Implementation of Data Parallelism and Task Parallelism
with a Multicore Processing Unit

Project Domain:
Operating Systems / Concurrent Programming / Multicore Processing

Project Objective:
To implement and demonstrate data parallelism and task parallelism
using a multicore processing approach, and compare their execution
performance through practical C++ programs and benchmarking.

Core learning objectives:

1. Understand process and thread concepts.
2. Understand thread creation and management.
3. Understand thread synchronization.
4. Demonstrate mutex-based mutual exclusion.
5. Understand data parallelism.
6. Understand task parallelism.
7. Implement a basic thread pool.
8. Compare sequential and parallel execution.
9. Measure execution time, speedup, and efficiency.
10. Store benchmark results in CSV files.
11. Display benchmark results through a web dashboard.
12. Understand the practical impact of thread count and scheduling.

The project is an educational implementation and benchmark.
Its measured results are specific to the development machine and
the recorded benchmark runs.

============================================================
2. GITHUB REPOSITORY AND VERSION CONTROL
============================================================

GitHub Username:
Vaayu0001

Repository Name:
MultiCore-Lab

Repository URL:
https://github.com/Vaayu0001/MultiCore-Lab

Git Remote:
origin

Remote URL:
https://github.com/Vaayu0001/MultiCore-Lab.git

Primary Branch:
main

Initial Commit:
53c99ee

Initial Commit Message:
Initial commit: MultiCore Lab project

Initial Commit Details:
19 files changed
2,399 insertions

Last verified Git state before creating this context file:
Branch: main
Tracking: origin/main
Working tree: clean
Local branch was up to date with origin/main.

Important:
The PROJECT_CONTEXT.md file is being added after the initial commit.
It must be committed and pushed separately after verification.

Git commands:

Check current branch:
git branch --show-current

Check working tree:
git status

Check remote:
git remote -v

Check latest commit:
git log -1 --oneline

Stage this context file:
git add PROJECT_CONTEXT.md

Commit:
git commit -m "Add complete project context and handoff documentation"

Push:
git push

Do not force-push.
Do not delete the existing repository.
Do not initialize a replacement repository.
Do not change the GitHub repository URL.

============================================================
3. DEVELOPMENT MACHINE AND SOFTWARE ENVIRONMENT
============================================================

Development Machine:
MacBook Air M1

Processor Architecture:
Apple Silicon / ARM64

Operating System:
macOS

Compiler:
Apple Clang 21.0.0 arm64

C++ Standard:
C++17

Build System:
CMake 4.4.5

Python Version:
Python 3.13.0

Python Web Framework:
Flask 3.1.3

Python Production Server:
Gunicorn (declared in requirements.txt)

Package Manager:
Homebrew

Homebrew Installation Path:
/opt/homebrew

Project Development Environment:
Visual Studio Code / Terminal

Do not assume that these installed versions are identical on a
future hosting machine.

============================================================
4. CORRECT PROJECT DIRECTORY
============================================================

The actual project directory is:

/Users/adarsh/Desktop/MultiCore-Lab

There is another directory:

/Users/adarsh/MultiCore-Lab

The second directory is an empty duplicate and must not be used.

Always verify the current project location with:

pwd

The expected output is:

/Users/adarsh/Desktop/MultiCore-Lab

============================================================
5. PROJECT TECHNOLOGY STACK
============================================================

C++17:
Used for multithreading, synchronization, parallel algorithms,
task execution, and benchmark implementations.

Standard C++ Threads:
Used for creating and managing concurrent worker threads.

Mutex:
Used to protect shared data and demonstrate mutual exclusion.

CMake:
Used to configure and build the C++ project.

Python:
Used for the web dashboard backend.

Flask:
Used to serve the dashboard and expose benchmark data through an API.

HTML:
Used to structure the dashboard interface.

CSS:
Used for dashboard styling and layout.

JavaScript:
Used for dashboard interactions and chart rendering.

Chart.js:
Used to visualize benchmark results.

CSV:
Used to store data parallelism and task parallelism benchmark results.

Gunicorn:
Intended to serve the Flask application in production on Render.

============================================================
6. PROJECT DIRECTORY STRUCTURE
============================================================

The verified and expected project structure is:

MultiCore-Lab/
|
|-- .git/
|
|-- .gitignore
|
|-- .vscode/
|
|-- CMakeLists.txt
|
|-- README.md
|
|-- requirements.txt
|
|-- PROJECT_CONTEXT.md
|
|-- src/
|   |
|   |-- main.cpp
|   |
|   |-- main_menu.cpp
|   |
|   |-- thread_basics.cpp
|   |
|   |-- mutex_basics.cpp
|   |
|   |-- data_parallelism/
|   |   |
|   |   |-- basic_data_parallelism.cpp
|   |   |
|   |   |-- data_parallelism_benchmark.cpp
|   |
|   |-- task_parallelism/
|       |
|       |-- basic_task_parallelism.cpp
|       |
|       |-- thread_pool.cpp
|       |
|       |-- task_parallelism_benchmark.cpp
|
|-- include/
|
|-- tests/
|
|-- benchmarks/
|
|-- results/
|   |
|   |-- data_parallelism.csv
|   |
|   |-- task_parallelism.csv
|
|-- dashboard/
|   |
|   |-- app.py
|   |
|   |-- venv/
|   |
|   |-- templates/
|       |
|       |-- index.html
|
|-- docs/
    |
    |-- PROJECT_DOCUMENTATION.md
    |
    |-- TESTING_REPORT.md

Important directory notes:

1. build/ contains local CMake build output.
2. dashboard/venv/ contains the local Python virtual environment.
3. build/ and dashboard/venv/ must not be committed to GitHub.
4. results/ contains important benchmark CSV files and must be committed.
5. The personal viva guide is stored outside the Git repository.
6. Do not create docs/VIVA_QUESTIONS.md unless explicitly requested.
7. Empty directories may not appear in Git because Git does not track
   empty directories.
8. Verify the actual structure before making structural changes.

============================================================
7. CMAKE CONFIGURATION AND BUILD TARGETS
============================================================

The project uses C++17.

CMake configuration uses:

find_package(Threads REQUIRED)

Thread-related targets link against:

Threads::Threads

The project has nine verified build targets:

1. thread_test
2. thread_basics
3. mutex_basics
4. basic_data_parallelism
5. data_parallelism_benchmark
6. basic_task_parallelism
7. thread_pool
8. task_parallelism_benchmark
9. multicore_lab

The complete CMake build previously completed successfully.

Verified build completion:
100%

All nine targets compiled successfully in the recorded test.

Standard CMake configuration command:

cmake -S . -B build

Standard build command:

cmake --build build

Before making CMake changes:
Inspect the current CMakeLists.txt.

Do not unnecessarily rename targets or modify the build configuration.

============================================================
8. IMPLEMENTED C++ PROGRAMS
============================================================

8.1 MAIN THREAD DEMONSTRATION

File:
src/main.cpp

Purpose:
Demonstrates basic thread creation and joining.

Concepts:
- Thread creation.
- Concurrent execution.
- Joining a thread.
- Waiting for thread completion.

8.2 THREAD BASICS

File:
src/thread_basics.cpp

Purpose:
Demonstrates three worker threads.

Concepts:
- Multiple threads.
- Independent execution.
- Thread lifecycle.
- Joining worker threads.

8.3 MUTEX SYNCHRONIZATION

File:
src/mutex_basics.cpp

Purpose:
Demonstrates synchronization of a shared counter.

Implementation:
Two threads increment a mutex-protected counter.

Expected final counter:
2000

Concepts:
- Shared data.
- Race conditions.
- Critical sections.
- Mutual exclusion.
- Mutex locking.

8.4 BASIC DATA PARALLELISM

File:
src/data_parallelism/basic_data_parallelism.cpp

Purpose:
Demonstrates dividing a large data-processing operation
among multiple worker threads.

Verified configuration:

ARRAY_SIZE = 10,000,000
NUM_THREADS = 4

Array initialization:

arr[i] = i % 100

Expected sum:

495,000,000

The sequential and parallel sums were verified to match.

A later basic demonstration reported efficiency of 154.545%.
This is a measured result from that particular run.
Do not artificially cap efficiency at 100%.

8.5 DATA PARALLELISM BENCHMARK

File:
src/data_parallelism/data_parallelism_benchmark.cpp

Purpose:
Measures sequential and parallel execution performance.

Recorded configuration:

Array size:
10,000,000

Warm-up runs:
2

Benchmark runs:
15

Thread counts:
1, 2, 4, 8

Timing method:
Median of repeated benchmark samples.

Correctness:
Sequential and parallel results are checked for equality.

Output CSV:
results/data_parallelism.csv

CSV columns:

array_size
warmup_runs
benchmark_runs
threads
sequential_ms
parallel_ms
speedup
efficiency
correctness

Recorded latest benchmark results:

Threads: 1
Sequential: 48.8162 ms
Parallel: 29.1533 ms
Speedup: 1.67446
Efficiency: 99.9314%
Correctness: PASSED

Threads: 2
Sequential: 47.7102 ms
Parallel: 16.9542 ms
Speedup: 2.81407
Efficiency: 87.1847%
Correctness: PASSED

Threads: 4
Sequential: 47.7870 ms
Parallel: 9.74304 ms
Speedup: 4.90473
Efficiency: 76.2084%
Correctness: PASSED

Threads: 8
Sequential: 47.0192 ms
Parallel: 9.92388 ms
Speedup: 4.73798
Efficiency: 37.2534%
Correctness: PASSED

Rounded presentation:

1 thread:
Sequential 48.82 ms
Parallel 29.15 ms
Speedup 1.67x
Efficiency 99.93%

2 threads:
Sequential 47.71 ms
Parallel 16.95 ms
Speedup 2.81x
Efficiency 87.18%

4 threads:
Sequential 47.79 ms
Parallel 9.74 ms
Speedup 4.90x
Efficiency 76.21%

8 threads:
Sequential 47.02 ms
Parallel 9.92 ms
Speedup 4.74x
Efficiency 37.25%

Interpretation:

Four threads produced the best recorded parallel execution time.

Eight threads were slightly slower than four threads.

Increasing thread count does not guarantee improved performance.

Possible reasons include scheduling overhead, synchronization,
resource contention, and hardware limitations.

Important efficiency formula used in this benchmark:

parallelSpeedup = singleWorkerTime / parallelTime

efficiency = parallelSpeedup / numThreads * 100

Do not confuse this formula with sequentialTime / parallelTime.

8.6 BASIC TASK PARALLELISM

File:
src/task_parallelism/basic_task_parallelism.cpp

Purpose:
Demonstrates independent tasks executing concurrently.

Input:

{10, 20, 15, 30, 25, 40}

Expected results:

Sum:
140

Factorial(5):
120

Maximum:
40

Even number count:
4

8.7 THREAD POOL

File:
src/task_parallelism/thread_pool.cpp

Purpose:
Demonstrates task execution using a fixed group of worker threads.

Recorded configuration:

Workers:
3

Tasks:
8

Expected task outputs:

30
25
60
24
100
40
49
100

Concepts:

- Worker threads.
- Task queue.
- Task submission.
- Concurrent execution.
- Synchronization.
- Thread reuse.
- Shutdown.
- Joining worker threads.

The destructor signals shutdown, drains queued work according
to the implementation, and joins the worker threads.

Do not claim that a thread pool is always faster than separate
threads. Performance depends on task size, worker count,
scheduling overhead, and workload.

8.8 TASK PARALLELISM BENCHMARK

File:
src/task_parallelism/task_parallelism_benchmark.cpp

Purpose:
Compares sequential task execution, separate threads,
and thread pool execution.

Recorded configuration:

Tasks:
8

Separate threads:
8

Thread pool workers:
3

Warm-up runs:
2

Benchmark runs:
15

Measurement:
Alternating measurement order and median timing.

Output CSV:
results/task_parallelism.csv

CSV columns:

tasks
workers
method
execution_ms
speedup
efficiency
correctness

Recorded results:

Sequential:
Execution: 75.219208 ms
Speedup: 1.0
Efficiency: N/A
Correctness: PASSED

Separate Threads:
Workers: 8
Execution: 16.367625 ms
Speedup: 4.595609
Efficiency: 57.501693%
Correctness: PASSED

Thread Pool:
Workers: 3
Execution: 29.939875 ms
Speedup: 2.512342
Efficiency: 83.814645%
Correctness: PASSED

Rounded presentation:

Sequential:
75.22 ms
1.00x speedup
Efficiency N/A

Separate Threads:
16.37 ms
4.60x speedup
57.50% efficiency

Thread Pool:
29.94 ms
2.51x speedup
83.81% efficiency

Interpretation:

Separate threads were faster in this recorded benchmark.

The thread pool had higher efficiency relative to its
single-worker pool baseline.

Do not compare efficiency values without explaining their
respective baselines.

============================================================
9. CENTRAL C++ MENU
============================================================

File:
src/main_menu.cpp

Purpose:
Provides a central interface for running project demonstrations.

Menu options:

1. Basic Data Parallelism
2. Data Parallelism Benchmark
3. Basic Task Parallelism
4. Thread Pool Demonstration
5. Task Parallelism Benchmark
0. Exit

All menu options were tested successfully in the recorded
full project test.

============================================================
10. BENCHMARK METHODOLOGY
============================================================

Data Parallelism:

Array size:
10,000,000

Warm-up:
2 runs

Measured runs:
15

Thread configurations:
1, 2, 4, 8

Statistic:
Median timing

Correctness:
Sequential and parallel sums are compared.

Task Parallelism:

Number of tasks:
8

Separate thread count:
8

Thread pool worker count:
3

Warm-up:
2 runs

Measured runs:
15

Measurement order:
Alternating order

Statistic:
Median timing

Correctness:
All recorded methods passed correctness checks.

Important interpretation rules:

1. Benchmark results are machine-specific.
2. Do not present historical timings as guaranteed future timings.
3. Do not invent results.
4. Do not change CSV values without rerunning the benchmark.
5. Keep the CSV files synchronized with the actual benchmark output.
6. Explain the efficiency baseline when discussing efficiency.
7. Distinguish execution time, speedup, and efficiency.

============================================================
11. CSV RESULT FILES
============================================================

Data Parallelism:

results/data_parallelism.csv

Task Parallelism:

results/task_parallelism.csv

Both files were present and verified.

Recorded sizes at last verification:

data_parallelism.csv:
322 bytes

task_parallelism.csv:
211 bytes

Both files contain benchmark results and correctness statuses.

These CSV files are required by the Flask dashboard.

They must remain included in Git.

============================================================
12. FLASK DASHBOARD
============================================================

Dashboard directory:

dashboard/

Backend:

dashboard/app.py

Frontend:

dashboard/templates/index.html

Python virtual environment:

dashboard/venv/

Dashboard purpose:

Display the recorded benchmark results through a browser-based
interface.

Dashboard components previously verified:

1. Overview.
2. Data parallelism charts.
3. Task parallelism charts.
4. Execution time visualization.
5. Speedup visualization.
6. Efficiency visualization.
7. Benchmark results table.

Chart rendering uses Chart.js.

12.1 FLASK APPLICATION STRUCTURE

The application imports:

from flask import Flask, jsonify, render_template
import csv
from pathlib import Path

Application object:

app = Flask(__name__)

Project root:

BASE_DIR = Path(__file__).resolve().parent.parent

Benchmark directory:

RESULTS_DIR = BASE_DIR / "results"

CSV reading:

The read_csv(filename) function constructs the CSV path
using RESULTS_DIR, checks whether the file exists,
opens it with newline="", uses csv.DictReader,
and returns the records as a list.

12.2 ROUTES

Homepage:

GET /

Function:
home()

Behavior:
Renders index.html.

Benchmark API:

GET /api/data

Function:
get_data()

Returns JSON containing:

data_parallelism
task_parallelism

The API reads:

results/data_parallelism.csv
results/task_parallelism.csv

12.3 LOCAL DASHBOARD TESTING

The following were previously verified:

Homepage returned HTTP 200.

API returned HTTP 200.

Dashboard displayed both benchmark datasets.

Dashboard refreshed correctly after benchmark CSV updates.

Recorded dashboard values included:

Array size:
10,000,000
Displayed as:
1,00,00,000

Data parallelism speedup:
4.90x

Separate thread task speedup:
4.60x

Thread pool speedup:
2.51x

Task execution times:

Sequential:
75.22 ms

Separate Threads:
16.37 ms

Thread Pool:
29.94 ms

A horizontal scrollbar in the benchmark results table was noticed.
It was a visual issue, not a functional failure.
An optional CSS improvement may be considered later.

============================================================
13. FLASK DEVELOPMENT SERVER NOTES
============================================================

The Flask application contains a development-server block:

if __name__ == "__main__":
    app.run(debug=True)

The exact current indentation and contents must be inspected
before modifying this section.

Do not use Flask's development server for production deployment.

The production deployment will use Gunicorn.

A previous macOS port issue occurred:

macOS Control Center was binding port 5000.

The recorded PID was 474 at that time.

Do not assume this PID is still current.

Do not kill macOS Control Center to resolve a port conflict.

The Flask server previously managed to start and serve HTTP 200
after the earlier conflict was resolved.

============================================================
14. PYTHON DEPENDENCIES
============================================================

Root file:

requirements.txt

Current recorded contents:

Flask==3.1.3
gunicorn

The file was created for Render deployment.

Do not replace the dependency list without checking the current
application and deployment requirements.

============================================================
15. GIT IGNORE CONFIGURATION
============================================================

The updated .gitignore contains:

# Build files
build/

# Python virtual environment
dashboard/venv/
venv/

# Python cache
__pycache__/
*.pyc

# macOS files
.DS_Store

# Editor files
.vscode/

Important:

Do not ignore results/*.csv.

The benchmark CSV files must be committed because the dashboard
uses them.

The personal viva PDF must not be committed.

============================================================
16. PROJECT DOCUMENTATION
============================================================

Root README:

README.md

Detailed project documentation:

docs/PROJECT_DOCUMENTATION.md

Testing report:

docs/TESTING_REPORT.md

The README was created and populated.

The project documentation covers:

- Project overview.
- Architecture.
- Data parallelism.
- Task parallelism.
- Thread pool.
- Benchmark methodology.
- Performance results.
- Dashboard.
- Testing.
- Conclusion.

The testing report covers:

- Compilation.
- Menu testing.
- Data parallelism.
- Task parallelism.
- Thread pool.
- Benchmark correctness.
- CSV outputs.
- Dashboard testing.

Documentation formatting was cleaned before the initial commit.

For future content intended to be pasted into Word:
Prefer plain-text field/value formatting if Markdown tables are inconvenient.

============================================================
17. PERSONAL VIVA GUIDE
============================================================

A separate personal viva PDF was generated:

MultiCore_Lab_Complete_Viva_Guide.pdf

Location:

/mnt/data/MultiCore_Lab_Complete_Viva_Guide.pdf

This file is outside the GitHub project repository.

It includes:

- OS fundamentals.
- Threads.
- Synchronization.
- Data parallelism.
- Task parallelism.
- Thread pools.
- Benchmark formulas.
- Recorded benchmark results.
- C++ implementation.
- Flask dashboard.
- Testing.
- Limitations.
- Rapid-fire viva questions.
- Answer framework.

Important:

The user explicitly wants personal viva preparation kept separate
from the project repository.

Do not add the viva PDF to GitHub.

Do not create docs/VIVA_QUESTIONS.md unless explicitly requested.

============================================================
18. FULL PROJECT TESTING STATUS
============================================================

The following testing was completed previously:

1. Full CMake build.
2. All nine CMake targets compiled.
3. Central menu tested.
4. Basic data parallelism tested.
5. Data parallelism benchmark tested.
6. Basic task parallelism tested.
7. Thread pool tested.
8. Task parallelism benchmark tested.
9. CSV output checked.
10. Correctness statuses checked.
11. Flask homepage tested.
12. Flask API tested.
13. Dashboard visualizations checked.
14. README created.
15. Project documentation created.
16. Testing report created.
17. GitHub repository created.
18. Initial Git commit created.
19. Initial push to GitHub succeeded.

All benchmark correctness statuses in the recorded CSV files were PASSED.

The local project was previously working.

Do not repeat all tests unnecessarily unless a change affects them.

============================================================
19. GITHUB PUSH HISTORY
============================================================

The local Git repository was initialized.

Initial commit:

53c99ee

Commit message:

Initial commit: MultiCore Lab project

The branch was renamed from master to main.

GitHub remote was added:

git remote add origin https://github.com/Vaayu0001/MultiCore-Lab.git

The initial push succeeded:

git push -u origin main

Verified remote:

origin
https://github.com/Vaayu0001/MultiCore-Lab.git

Verified branch:

main

The repository was clean and up to date after the push.

============================================================
20. RENDER DEPLOYMENT — CURRENT PENDING TASK
============================================================

Deployment platform:

Render

Deployment type:

Web Service

Application to deploy:

Flask dashboard only.

The C++ programs remain source code and local executable
demonstrations in the GitHub repository.

They are not being deployed as a web application on Render.

GitHub repository:

https://github.com/Vaayu0001/MultiCore-Lab

Expected Render configuration:

Repository:
Vaayu0001/MultiCore-Lab

Root Directory:
Leave blank.
The repository root contains requirements.txt and dashboard/.

Build Command:

pip install -r requirements.txt

Start Command:

gunicorn --chdir dashboard --bind 0.0.0.0:$PORT app:app

Reason for the start command:

1. Gunicorn runs the production server.
2. --chdir dashboard makes dashboard the working directory.
3. app:app imports the app object from dashboard/app.py.
4. --bind 0.0.0.0:$PORT listens on the Render-provided port.
5. The Flask application calculates BASE_DIR using the parent
   of the dashboard directory, so it can still locate root-level
   results/ CSV files.

This is the intended deployment configuration.
Verify the actual repository files and Render settings before deployment.

Deployment status:

NOT DEPLOYED.

No Render public URL exists yet.

Next deployment tasks:

1. Open Render dashboard.
2. Sign in using GitHub.
3. Authorize repository access if required.
4. Select New Web Service.
5. Choose Vaayu0001/MultiCore-Lab.
6. Configure the build command.
7. Configure the start command.
8. Keep root directory blank.
9. Review environment settings.
10. Deploy.
11. Inspect build logs.
12. Inspect runtime logs.
13. Verify the homepage.
14. Verify /api/data.
15. Verify both benchmark datasets appear correctly.
16. Record the public Render URL in this file.

Important hosting note:

Render free services may spin down after inactivity.
A cold start may occur when the service is accessed again.

GitHub Pages cannot host the Flask backend.
Render is being used because the application requires Python server-side execution.

============================================================
21. CURRENT PROJECT STATUS SUMMARY
============================================================

COMPLETED:

[YES] Project planning and implementation.
[YES] C++17 setup.
[YES] Thread basics.
[YES] Mutex synchronization.
[YES] Basic data parallelism.
[YES] Data parallelism benchmark.
[YES] Basic task parallelism.
[YES] Thread pool.
[YES] Task parallelism benchmark.
[YES] Central menu.
[YES] CMake build.
[YES] Benchmark CSV files.
[YES] Flask backend.
[YES] HTML dashboard.
[YES] Chart.js visualizations.
[YES] Local dashboard testing.
[YES] README.
[YES] Project documentation.
[YES] Testing report.
[YES] Personal viva guide outside repository.
[YES] GitHub repository.
[YES] Initial Git commit.
[YES] Initial GitHub push.

PENDING:

[ ] Commit this PROJECT_CONTEXT.md file.
[ ] Push the context file to GitHub.
[ ] Connect Render to GitHub.
[ ] Configure Render Web Service.
[ ] Deploy Flask dashboard.
[ ] Verify production homepage.
[ ] Verify production API.
[ ] Verify live benchmark charts.
[ ] Record final public URL.

============================================================
22. FUTURE CHANGE CONTROL
============================================================

Before changing any project file:

1. Confirm the correct project directory.
2. Inspect the current file.
3. Understand its role in the architecture.
4. Identify whether the change affects C++, CMake, Flask,
   dashboard frontend, CSV data, or deployment.
5. Avoid unnecessary changes to working components.
6. Provide the exact file path.
7. Provide complete replacement code when replacement is required.
8. Do not provide incomplete patches without explaining exactly
   where they belong.
9. Preserve existing benchmark methodology.
10. Preserve recorded results unless new benchmarks are run.
11. Rebuild affected C++ targets after C++ changes.
12. Run Python syntax checks after Flask changes.
13. Test affected routes after backend changes.
14. Verify dashboard behavior after frontend changes.
15. Check Git diff before committing.
16. Never force-push without an explicit reason and user approval.

Never claim a test, build, deployment, or push succeeded
without receiving or independently verifying its result.

============================================================
23. USER WORKING PREFERENCES
============================================================

The user is an engineering student studying Computer Science
and Engineering.

The user prefers:

1. Beginner-friendly technical explanations.
2. Step-by-step guidance.
3. One meaningful action at a time.
4. Exact terminal commands.
5. Exact file paths.
6. Complete code when code replacement is needed.
7. Verification before proceeding.
8. Correct calculations and expected outputs.
9. No unnecessary complexity.
10. No unnecessary new features.
11. No assumptions about successful execution.
12. No repeating work that is already complete.
13. Clear explanations of errors and their solutions.
14. Professional and encouraging communication.

The user has explicitly emphasized:

"Do not ever give wrong code or answer."

Therefore:

- Verify code before sharing it.
- Verify calculations.
- Distinguish measured results from expected results.
- Do not guess command output.
- Ask for terminal output when verification is required.
- Never ask the user to rebuild the project from scratch
  when the existing project is already available.

============================================================
24. HOW A NEW CHAT MUST CONTINUE
============================================================

When a new chat starts under the OS LAB project:

1. Read this entire PROJECT_CONTEXT.md file.
2. Treat the existing GitHub repository as the source of project files.
3. Treat the benchmark values in this file as historical verified results.
4. Do not claim to have inspected the current repository unless it
   has actually been inspected.
5. Do not ask the user to upload a ZIP file by default.
6. Do not ask the user to explain the project again.
7. Do not rebuild the project from scratch.
8. Do not modify working C++ code unnecessarily.
9. Continue from the pending tasks in Section 21.
10. The immediate next task is Render deployment.

If a file needs inspection, request only that specific file or
inspect the public GitHub repository.

If a new deployment result or project decision is verified,
update this file so that future chats have the latest information.

============================================================
25. IMMEDIATE NEXT ACTION
============================================================

Continue with Render deployment.

The project is already on GitHub.

The C++ implementation, benchmarking, dashboard, documentation,
and local testing are complete.

Do not restart development.

The next objective is:

Deploy the existing Flask dashboard on Render and obtain
a working public website URL.

END OF PROJECT CONTEXT
