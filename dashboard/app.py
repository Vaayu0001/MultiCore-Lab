from flask import Flask, jsonify, render_template
import csv
from pathlib import Path

app = Flask(__name__)

# Get the main project directory
BASE_DIR = Path(__file__).resolve().parent.parent

# Location of benchmark CSV files
RESULTS_DIR = BASE_DIR / "results"


# Read CSV files
def read_csv(filename):
    file_path = RESULTS_DIR / filename

    if not file_path.exists():
        return []

    with open(file_path, "r", newline="") as file:
        reader = csv.DictReader(file)
        return list(reader)


# Main dashboard page
@app.route("/")
def home():
    return render_template("index.html")


# API to provide benchmark data
@app.route("/api/data")
def get_data():

    data = {
        "data_parallelism": read_csv("data_parallelism.csv"),
        "task_parallelism": read_csv("task_parallelism.csv")
    }

    return jsonify(data)


# Start Flask server
if __name__ == "__main__":
  app.run(debug=True)