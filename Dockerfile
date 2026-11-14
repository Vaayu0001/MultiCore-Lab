FROM python:3.13-slim

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

COPY . .

RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build \
       --target data_parallelism_benchmark task_parallelism_benchmark \
       -j2

ENV PYTHONUNBUFFERED=1
ENV ENABLE_BENCHMARKS=true

ENV MAX_ARRAY_SIZE=2000000
ENV MAX_WORKERS=4
ENV MAX_WARMUP_RUNS=1
ENV MAX_BENCHMARK_RUNS=3
ENV MAX_TASKS=8
ENV MAX_WORK_PER_TASK=500000
ENV MAX_TASK_TOTAL_WORK=2000000
ENV BENCHMARK_TIMEOUT=45

CMD ["sh", "-c", "gunicorn --workers 1 --threads 4 --timeout 120 --chdir dashboard --bind 0.0.0.0:${PORT:-10000} app:app"]
