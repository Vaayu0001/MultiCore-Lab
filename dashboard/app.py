from flask import Flask, jsonify, render_template, request
import csv
import json
import os
import subprocess
import threading
from pathlib import Path

app = Flask(__name__)

BASE_DIR = Path(__file__).resolve().parent.parent
RESULTS_DIR = BASE_DIR / "results"
BUILD_DIR = BASE_DIR / "build"

DATA_PROGRAM = BUILD_DIR / "data_parallelism_benchmark"
TASK_PROGRAM = BUILD_DIR / "task_parallelism_benchmark"

ENABLE_BENCHMARKS = (
    os.environ.get("ENABLE_BENCHMARKS", "false").lower() == "true"
)

benchmark_lock = threading.Lock()

DATA_COLUMNS = [
    "array_size",
    "warmup_runs",
    "benchmark_runs",
    "threads",
    "sequential_ms",
    "parallel_ms",
    "speedup",
    "efficiency",
    "correctness"
]

TASK_COLUMNS = [
    "tasks",
    "workers",
    "method",
    "execution_ms",
    "speedup",
    "efficiency",
    "correctness"
]


def read_csv(filename):
    file_path = RESULTS_DIR / filename

    if not file_path.exists():
        return []

    with open(file_path, "r", newline="") as file:
        reader = csv.DictReader(file)
        return list(reader)


def save_csv(filename, columns, rows):
    RESULTS_DIR.mkdir(parents=True, exist_ok=True)

    file_path = RESULTS_DIR / filename
    write_header = (
        not file_path.exists() or file_path.stat().st_size == 0
    )

    with open(file_path, "a", newline="") as file:
        writer = csv.DictWriter(file, fieldnames=columns)

        if write_header:
            writer.writeheader()

        writer.writerows(rows)


def read_integer(data, name, minimum, maximum):
    value = data.get(name)

    if isinstance(value, bool) or not isinstance(value, int):
        raise ValueError(f"{name} must be an integer")

    if value < minimum or value > maximum:
        raise ValueError(
            f"{name} must be between {minimum} and {maximum}"
        )

    return value


def read_workers(data, name, maximum):
    value = data.get(name)

    if not isinstance(value, list) or not value:
        raise ValueError(f"{name} must be a non-empty list")

    if any(
        isinstance(worker, bool) or not isinstance(worker, int)
        for worker in value
    ):
        raise ValueError("Worker counts must be integers")

    if len(value) != len(set(value)):
        raise ValueError("Duplicate worker counts are not allowed")

    if any(worker < 1 or worker > maximum for worker in value):
        raise ValueError(
            f"Worker counts must be between 1 and {maximum}"
        )

    return value


def run_program(program, arguments):
    if not ENABLE_BENCHMARKS:
        raise PermissionError("Benchmark execution is disabled")

    if not program.is_file():
        raise FileNotFoundError(
            f"Benchmark executable not found: {program.name}"
        )

    result = subprocess.run(
        [str(program)] + [str(arg) for arg in arguments],
        capture_output=True,
        text=True,
        timeout=300,
        cwd=BASE_DIR,
        check=False
    )

    if result.returncode != 0:
        raise RuntimeError(
            result.stderr.strip() or "Benchmark execution failed"
        )

    try:
        output = json.loads(result.stdout)
    except json.JSONDecodeError:
        raise RuntimeError("Benchmark returned invalid JSON")

    return output


@app.route("/")
def home():
    return render_template("index.html")


@app.route("/api/data")
def get_data():
    data = {
        "data_parallelism": read_csv("data_parallelism.csv"),
        "task_parallelism": read_csv("task_parallelism.csv")
    }

    return jsonify(data)


@app.route("/api/run/data", methods=["POST"])
def run_data_benchmark():
    if not ENABLE_BENCHMARKS:
        return jsonify({
            "error": "Benchmark execution is disabled on this server"
        }), 403

    if not benchmark_lock.acquire(blocking=False):
        return jsonify({
            "error": "Another benchmark is already running"
        }), 409

    try:
        data = request.get_json(silent=True)

        if not isinstance(data, dict):
            return jsonify({"error": "Invalid JSON request"}), 400

        array_size = read_integer(
            data, "array_size", 1000, 50000000
        )

        warmup_runs = read_integer(
            data, "warmup_runs", 0, 20
        )

        benchmark_runs = read_integer(
            data, "benchmark_runs", 1, 100
        )

        workers = read_workers(data, "workers", 256)

        output = run_program(
            DATA_PROGRAM,
            [
                array_size,
                warmup_runs,
                benchmark_runs,
                ",".join(map(str, workers))
            ]
        )

        if output.get("benchmark") != "data_parallelism":
            raise RuntimeError("Unexpected benchmark output")

        results = output.get("results")

        if not isinstance(results, list) or not results:
            raise RuntimeError("Benchmark returned no results")

        if any(row.get("correctness") != "PASSED" for row in results):
            raise RuntimeError("Benchmark correctness verification failed")

        rows = []

        for result in results:
            rows.append({
                "array_size": output["array_size"],
                "warmup_runs": output["warmup_runs"],
                "benchmark_runs": output["benchmark_runs"],
                "threads": result["threads"],
                "sequential_ms": result["sequential_ms"],
                "parallel_ms": result["parallel_ms"],
                "speedup": result["speedup"],
                "efficiency": result["efficiency"],
                "correctness": result["correctness"]
            })

        save_csv("data_parallelism.csv", DATA_COLUMNS, rows)

        return jsonify({
            "message": "Data-parallelism benchmark completed",
            "results": rows
        })

    except ValueError as error:
        return jsonify({"error": str(error)}), 400

    except subprocess.TimeoutExpired:
        return jsonify({"error": "Benchmark timed out"}), 504

    except (FileNotFoundError, PermissionError, RuntimeError) as error:
        return jsonify({"error": str(error)}), 500

    finally:
        benchmark_lock.release()


@app.route("/api/run/task", methods=["POST"])
def run_task_benchmark():
    if not ENABLE_BENCHMARKS:
        return jsonify({
            "error": "Benchmark execution is disabled on this server"
        }), 403

    if not benchmark_lock.acquire(blocking=False):
        return jsonify({
            "error": "Another benchmark is already running"
        }), 409

    try:
        data = request.get_json(silent=True)

        if not isinstance(data, dict):
            return jsonify({"error": "Invalid JSON request"}), 400

        tasks = read_integer(data, "tasks", 1, 64)

        work = read_integer(
            data, "work_per_task", 1000, 10000000
        )

        if tasks * work > 100000000:
            raise ValueError(
                "Total workload cannot exceed 100 million iterations"
            )

        warmup_runs = read_integer(
            data, "warmup_runs", 0, 20
        )

        benchmark_runs = read_integer(
            data, "benchmark_runs", 1, 100
        )

        workers = read_workers(data, "workers", tasks)

        output = run_program(
            TASK_PROGRAM,
            [
                tasks,
                work,
                warmup_runs,
                benchmark_runs,
                ",".join(map(str, workers))
            ]
        )

        if output.get("benchmark") != "task_parallelism":
            raise RuntimeError("Unexpected benchmark output")

        results = output.get("results")

        if not isinstance(results, list) or not results:
            raise RuntimeError("Benchmark returned no results")

        if any(row.get("correctness") != "PASSED" for row in results):
            raise RuntimeError("Benchmark correctness verification failed")

        rows = []

        for result in results:
            rows.append({
                "tasks": output["tasks"],
                "workers": result["workers"],
                "method": result["method"],
                "execution_ms": result["execution_ms"],
                "speedup": result["speedup"],
                "efficiency": (
                    "" if result["efficiency"] is None
                    else result["efficiency"]
                ),
                "correctness": result["correctness"]
            })

        save_csv("task_parallelism.csv", TASK_COLUMNS, rows)

        return jsonify({
            "message": "Task-parallelism benchmark completed",
            "results": rows
        })

    except ValueError as error:
        return jsonify({"error": str(error)}), 400

    except subprocess.TimeoutExpired:
        return jsonify({"error": "Benchmark timed out"}), 504

    except (FileNotFoundError, PermissionError, RuntimeError) as error:
        return jsonify({"error": str(error)}), 500

    finally:
        benchmark_lock.release()


if __name__ == "__main__":
    app.run(debug=True)