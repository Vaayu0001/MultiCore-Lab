from flask import Flask, jsonify, render_template, request
import csv
import json
import os
import platform
import shutil
import subprocess
import threading
import uuid
from datetime import datetime, timezone
from pathlib import Path

app = Flask(__name__)

BASE_DIR = Path(__file__).resolve().parent.parent
RESULTS_DIR = BASE_DIR / "results"
BUILD_DIR = BASE_DIR / "build"
EXPERIMENTS_FILE = RESULTS_DIR / "experiments.json"

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
    "correctness",
    "experiment_id"
]

TASK_COLUMNS = [
    "tasks",
    "workers",
    "method",
    "execution_ms",
    "speedup",
    "efficiency",
    "correctness",
    "experiment_id"
]


# ---------------- BACKUP MANAGEMENT ----------------

def create_backup():

    RESULTS_DIR.mkdir(parents=True, exist_ok=True)

    backup_dir = RESULTS_DIR / "backups"
    backup_dir.mkdir(exist_ok=True)

    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S_%f")

    backup_path = backup_dir / f"backup_{timestamp}"
    backup_path.mkdir()

    files = [
        EXPERIMENTS_FILE,
        RESULTS_DIR / "data_parallelism.csv",
        RESULTS_DIR / "task_parallelism.csv"
    ]

    for file_path in files:

        if file_path.exists():
            shutil.copy2(file_path, backup_path / file_path.name)

    return backup_path


def restore_backup(backup_path):

    files = [
        EXPERIMENTS_FILE,
        RESULTS_DIR / "data_parallelism.csv",
        RESULTS_DIR / "task_parallelism.csv"
    ]

    for file_path in files:

        backup_file = backup_path / file_path.name

        if backup_file.exists():
            shutil.copy2(backup_file, file_path)

        elif file_path.exists():
            file_path.unlink()


# ---------------- CSV MANAGEMENT ----------------

def read_csv(filename):

    file_path = RESULTS_DIR / filename

    if not file_path.exists():
        return []

    with open(file_path, "r", newline="", encoding="utf-8") as file:
        reader = csv.DictReader(file)
        return list(reader)


def ensure_csv_columns(filename, columns):
    """Add new columns without deleting existing historical records."""

    file_path = RESULTS_DIR / filename

    if not file_path.exists() or file_path.stat().st_size == 0:
        return

    with open(file_path, "r", newline="", encoding="utf-8") as file:
        reader = csv.DictReader(file)
        existing_columns = reader.fieldnames or []
        existing_rows = list(reader)

    if all(column in existing_columns for column in columns):
        return

    updated_columns = list(existing_columns)

    for column in columns:
        if column not in updated_columns:
            updated_columns.append(column)

    temporary_file = file_path.with_suffix(
        file_path.suffix + ".tmp"
    )

    with open(
        temporary_file, "w", newline="", encoding="utf-8"
    ) as file:
        writer = csv.DictWriter(file, fieldnames=updated_columns)
        writer.writeheader()
        writer.writerows(existing_rows)

    os.replace(temporary_file, file_path)


def save_csv(filename, columns, rows):

    RESULTS_DIR.mkdir(parents=True, exist_ok=True)

    ensure_csv_columns(filename, columns)

    file_path = RESULTS_DIR / filename

    write_header = (
        not file_path.exists() or file_path.stat().st_size == 0
    )

    with open(file_path, "a", newline="", encoding="utf-8") as file:
        writer = csv.DictWriter(file, fieldnames=columns)

        if write_header:
            writer.writeheader()

        writer.writerows(rows)


# ---------------- EXPERIMENT MANAGEMENT ----------------

def read_experiments():

    if not EXPERIMENTS_FILE.exists():
        return []

    try:
        with open(EXPERIMENTS_FILE, "r", encoding="utf-8") as file:
            experiments = json.load(file)

    except (OSError, json.JSONDecodeError) as error:
        raise RuntimeError(
            "Experiment metadata file could not be read"
        ) from error

    if not isinstance(experiments, list):
        raise RuntimeError(
            "Experiment metadata file has an invalid format"
        )

    return experiments


def save_experiments(experiments):

    RESULTS_DIR.mkdir(parents=True, exist_ok=True)

    temporary_file = EXPERIMENTS_FILE.with_suffix(".json.tmp")

    with open(temporary_file, "w", encoding="utf-8") as file:
        json.dump(experiments, file, indent=2)
        file.write("\n")

    os.replace(temporary_file, EXPERIMENTS_FILE)


def get_hardware_info():

    return {
        "processor": platform.processor() or "Unknown",
        "architecture": platform.machine() or "Unknown",
        "operating_system": platform.system(),
        "os_release": platform.release(),
        "logical_cpu_count": os.cpu_count()
    }


def create_experiment(experiment_type, configuration):

    experiment = {
        "experiment_id": str(uuid.uuid4()),
        "timestamp": datetime.now(timezone.utc).isoformat(),
        "experiment_type": experiment_type,
        "configuration": configuration,
        "hardware_info": get_hardware_info(),
        "status": "Running",
        "result_count": 0,
        "error": None
    }

    experiments = read_experiments()
    experiments.append(experiment)

    save_experiments(experiments)

    return experiment


def finish_experiment(
    experiment,
    status,
    result_count=0,
    error=None
):

    experiments = read_experiments()

    for saved_experiment in experiments:

        if (
            saved_experiment.get("experiment_id")
            == experiment["experiment_id"]
        ):

            saved_experiment["status"] = status
            saved_experiment["result_count"] = result_count

            saved_experiment["completed_at"] = (
                datetime.now(timezone.utc).isoformat()
            )

            saved_experiment["error"] = error
            break

    else:
        raise RuntimeError(
            "Experiment metadata record was not found"
        )

    save_experiments(experiments)


# ---------------- INPUT VALIDATION ----------------

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
        raise ValueError(
            f"{name} must be a non-empty list"
        )

    if any(
        isinstance(worker, bool) or not isinstance(worker, int)
        for worker in value
    ):
        raise ValueError(
            "Worker counts must be integers"
        )

    if len(value) != len(set(value)):
        raise ValueError(
            "Duplicate worker counts are not allowed"
        )

    if any(worker < 1 or worker > maximum for worker in value):
        raise ValueError(
            f"Worker counts must be between 1 and {maximum}"
        )

    return value


# ---------------- C++ EXECUTION ----------------

def run_program(program, arguments):

    if not ENABLE_BENCHMARKS:
        raise PermissionError(
            "Benchmark execution is disabled"
        )

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

    except json.JSONDecodeError as error:
        raise RuntimeError(
            "Benchmark returned invalid JSON"
        ) from error

    return output


# ---------------- DASHBOARD ROUTES ----------------

@app.route("/")
def home():

    return render_template("index.html")


@app.route("/api/data")
def get_data():

    data = {
        "data_parallelism": read_csv(
            "data_parallelism.csv"
        ),
        "task_parallelism": read_csv(
            "task_parallelism.csv"
        )
    }

    return jsonify(data)


@app.route("/api/experiments")
def get_experiments():

    try:
        return jsonify(read_experiments())

    except RuntimeError as error:
        return jsonify({"error": str(error)}), 500


# ---------------- DELETE EXPERIMENT ----------------

@app.route("/api/experiments/<experiment_id>", methods=["DELETE"])
def delete_experiment(experiment_id):

    if not benchmark_lock.acquire(blocking=False):
        return jsonify({
            "error": "Cannot delete while a benchmark is running"
        }), 409

    backup_path = None
    temporary_files = []

    try:

        experiments = read_experiments()

        experiment = next(
            (
                item for item in experiments
                if item.get("experiment_id") == experiment_id
            ),
            None
        )

        if experiment is None:
            return jsonify({
                "error": "Experiment not found"
            }), 404

        experiment_type = experiment.get("experiment_type")

        if experiment_type == "data_parallelism":
            csv_file = RESULTS_DIR / "data_parallelism.csv"
            default_columns = DATA_COLUMNS

        elif experiment_type == "task_parallelism":
            csv_file = RESULTS_DIR / "task_parallelism.csv"
            default_columns = TASK_COLUMNS

        else:
            return jsonify({
                "error": "Invalid experiment type"
            }), 400

        backup_path = create_backup()

        updated_experiments = [
            item for item in experiments
            if item.get("experiment_id") != experiment_id
        ]

        csv_temp = None

        if csv_file.exists():

            with open(
                csv_file, "r", newline="", encoding="utf-8"
            ) as file:
                reader = csv.DictReader(file)
                columns = reader.fieldnames or default_columns
                rows = list(reader)

            updated_rows = [
                row for row in rows
                if row.get("experiment_id") != experiment_id
            ]

            csv_temp = csv_file.with_name(
                csv_file.name + ".delete.tmp"
            )

            temporary_files.append(csv_temp)

            with open(
                csv_temp, "w", newline="", encoding="utf-8"
            ) as file:

                writer = csv.DictWriter(
                    file,
                    fieldnames=columns
                )

                writer.writeheader()
                writer.writerows(updated_rows)

        json_temp = EXPERIMENTS_FILE.with_name(
            EXPERIMENTS_FILE.name + ".delete.tmp"
        )

        temporary_files.append(json_temp)

        with open(json_temp, "w", encoding="utf-8") as file:
            json.dump(updated_experiments, file, indent=2)
            file.write("\n")

        if csv_temp is not None:
            os.replace(csv_temp, csv_file)

        os.replace(json_temp, EXPERIMENTS_FILE)

        return jsonify({
            "message": "Experiment deleted successfully",
            "experiment_id": experiment_id,
            "backup": str(backup_path)
        })

    except Exception:

        app.logger.exception("Experiment deletion failed")

        if backup_path is not None:

            try:
                restore_backup(backup_path)

            except Exception:
                app.logger.exception(
                    "CRITICAL: Could not restore experiment backup"
                )

                return jsonify({
                    "error": (
                        "Deletion failed and automatic rollback "
                        "also failed. Restore the backup manually."
                    ),
                    "backup": str(backup_path)
                }), 500

        return jsonify({
            "error": "Experiment deletion failed. Original data was restored."
        }), 500

    finally:

        for temporary_file in temporary_files:

            try:
                if temporary_file.exists():
                    temporary_file.unlink()

            except OSError:
                app.logger.exception(
                    "Could not remove temporary file: %s",
                    temporary_file
                )

        benchmark_lock.release()


# ---------------- CLEAR HISTORY ----------------

@app.route("/api/experiments", methods=["DELETE"])
def clear_history():

    data = request.get_json(silent=True)

    if not isinstance(data, dict):
        return jsonify({
            "error": "Invalid JSON request"
        }), 400

    clear_type = data.get("experiment_type")

    if clear_type not in [
        "data_parallelism",
        "task_parallelism",
        "all"
    ]:
        return jsonify({
            "error": "Invalid experiment type"
        }), 400

    if not benchmark_lock.acquire(blocking=False):
        return jsonify({
            "error": "Cannot clear history while a benchmark is running"
        }), 409

    backup_path = None
    temporary_files = []

    try:

        experiments = read_experiments()

        if clear_type == "all":

            updated_experiments = []

            csv_files = [
                (
                    RESULTS_DIR / "data_parallelism.csv",
                    DATA_COLUMNS
                ),
                (
                    RESULTS_DIR / "task_parallelism.csv",
                    TASK_COLUMNS
                )
            ]

        else:

            updated_experiments = [
                item for item in experiments
                if item.get("experiment_type") != clear_type
            ]

            if clear_type == "data_parallelism":

                csv_files = [
                    (
                        RESULTS_DIR / "data_parallelism.csv",
                        DATA_COLUMNS
                    )
                ]

            else:

                csv_files = [
                    (
                        RESULTS_DIR / "task_parallelism.csv",
                        TASK_COLUMNS
                    )
                ]

        backup_path = create_backup()

        cleared_results = 0
        csv_updates = []

        # Prepare empty CSV files while preserving their headers.

        for csv_file, default_columns in csv_files:

            if not csv_file.exists():
                continue

            with open(
                csv_file, "r", newline="", encoding="utf-8"
            ) as file:

                reader = csv.DictReader(file)
                columns = reader.fieldnames or default_columns
                rows = list(reader)

            cleared_results += len(rows)

            csv_temp = csv_file.with_name(
                csv_file.name + ".clear.tmp"
            )

            temporary_files.append(csv_temp)

            with open(
                csv_temp, "w", newline="", encoding="utf-8"
            ) as file:

                writer = csv.DictWriter(
                    file,
                    fieldnames=columns
                )

                writer.writeheader()

            csv_updates.append((csv_temp, csv_file))

        # Prepare updated experiment metadata.

        json_temp = EXPERIMENTS_FILE.with_name(
            EXPERIMENTS_FILE.name + ".clear.tmp"
        )

        temporary_files.append(json_temp)

        with open(json_temp, "w", encoding="utf-8") as file:
            json.dump(updated_experiments, file, indent=2)
            file.write("\n")

        # Replace CSV files first, then metadata.

        for csv_temp, csv_file in csv_updates:
            os.replace(csv_temp, csv_file)

        os.replace(json_temp, EXPERIMENTS_FILE)

        cleared_experiments = (
            len(experiments) - len(updated_experiments)
        )

        return jsonify({
            "message": "History cleared successfully",
            "experiment_type": clear_type,
            "cleared_experiments": cleared_experiments,
            "cleared_results": cleared_results,
            "backup": str(backup_path)
        })

    except Exception:

        app.logger.exception("Clear history operation failed")

        if backup_path is not None:

            try:
                restore_backup(backup_path)

            except Exception:

                app.logger.exception(
                    "CRITICAL: Could not restore backup"
                )

                return jsonify({
                    "error": (
                        "Clear failed and automatic rollback failed. "
                        "Restore the backup manually."
                    ),
                    "backup": str(backup_path)
                }), 500

        return jsonify({
            "error": (
                "Could not clear history. "
                "Original data was restored."
            )
        }), 500

    finally:

        for temporary_file in temporary_files:

            try:
                if temporary_file.exists():
                    temporary_file.unlink()

            except OSError:
                app.logger.exception(
                    "Could not remove temporary file: %s",
                    temporary_file
                )

        benchmark_lock.release()


# ---------------- DATA PARALLELISM ----------------

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

    experiment = None

    try:

        data = request.get_json(silent=True)

        if not isinstance(data, dict):
            return jsonify({
                "error": "Invalid JSON request"
            }), 400

        array_size = read_integer(
            data, "array_size", 1000, 50000000
        )

        warmup_runs = read_integer(
            data, "warmup_runs", 0, 20
        )

        benchmark_runs = read_integer(
            data, "benchmark_runs", 1, 100
        )

        workers = read_workers(
            data, "workers", 256
        )

        experiment = create_experiment(
            "data_parallelism",
            {
                "array_size": array_size,
                "warmup_runs": warmup_runs,
                "benchmark_runs": benchmark_runs,
                "workers": workers
            }
        )

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
            raise RuntimeError(
                "Unexpected benchmark output"
            )

        results = output.get("results")

        if not isinstance(results, list) or not results:
            raise RuntimeError(
                "Benchmark returned no results"
            )

        if any(
            row.get("correctness") != "PASSED"
            for row in results
        ):
            raise RuntimeError(
                "Benchmark correctness verification failed"
            )

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
                "correctness": result["correctness"],
                "experiment_id": experiment["experiment_id"]
            })

        save_csv(
            "data_parallelism.csv",
            DATA_COLUMNS,
            rows
        )

        finish_experiment(
            experiment,
            "Completed",
            len(rows)
        )

        return jsonify({
            "message": "Data-parallelism benchmark completed",
            "experiment_id": experiment["experiment_id"],
            "results": rows
        })

    except ValueError as error:

        return jsonify({
            "error": str(error)
        }), 400

    except subprocess.TimeoutExpired:

        if experiment is not None:
            finish_experiment(
                experiment,
                "Failed",
                error="Benchmark timed out"
            )

        return jsonify({
            "error": "Benchmark timed out"
        }), 504

    except Exception as error:

        if experiment is not None:
            try:
                finish_experiment(
                    experiment,
                    "Failed",
                    error=str(error)
                )

            except Exception:
                app.logger.exception(
                    "Could not update failed experiment metadata"
                )

        app.logger.exception(
            "Data-parallelism benchmark failed"
        )

        return jsonify({
            "error": str(error)
        }), 500

    finally:
        benchmark_lock.release()


# ---------------- TASK PARALLELISM ----------------

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

    experiment = None

    try:

        data = request.get_json(silent=True)

        if not isinstance(data, dict):
            return jsonify({
                "error": "Invalid JSON request"
            }), 400

        tasks = read_integer(
            data, "tasks", 1, 64
        )

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

        workers = read_workers(
            data, "workers", tasks
        )

        experiment = create_experiment(
            "task_parallelism",
            {
                "tasks": tasks,
                "work_per_task": work,
                "warmup_runs": warmup_runs,
                "benchmark_runs": benchmark_runs,
                "workers": workers
            }
        )

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
            raise RuntimeError(
                "Unexpected benchmark output"
            )

        results = output.get("results")

        if not isinstance(results, list) or not results:
            raise RuntimeError(
                "Benchmark returned no results"
            )

        if any(
            row.get("correctness") != "PASSED"
            for row in results
        ):
            raise RuntimeError(
                "Benchmark correctness verification failed"
            )

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
                "correctness": result["correctness"],
                "experiment_id": experiment["experiment_id"]
            })

        save_csv(
            "task_parallelism.csv",
            TASK_COLUMNS,
            rows
        )

        finish_experiment(
            experiment,
            "Completed",
            len(rows)
        )

        return jsonify({
            "message": "Task-parallelism benchmark completed",
            "experiment_id": experiment["experiment_id"],
            "results": rows
        })

    except ValueError as error:

        return jsonify({
            "error": str(error)
        }), 400

    except subprocess.TimeoutExpired:

        if experiment is not None:
            finish_experiment(
                experiment,
                "Failed",
                error="Benchmark timed out"
            )

        return jsonify({
            "error": "Benchmark timed out"
        }), 504

    except Exception as error:

        if experiment is not None:
            try:
                finish_experiment(
                    experiment,
                    "Failed",
                    error=str(error)
                )

            except Exception:
                app.logger.exception(
                    "Could not update failed experiment metadata"
                )

        app.logger.exception(
            "Task-parallelism benchmark failed"
        )

        return jsonify({
            "error": str(error)
        }), 500

    finally:
        benchmark_lock.release()


if __name__ == "__main__":
    app.run(debug=True)