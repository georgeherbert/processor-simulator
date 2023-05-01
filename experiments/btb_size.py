import subprocess
import re
from concurrent.futures import ThreadPoolExecutor

DIRECTORY = "../src_superscalar"
BENCHMARKS = [
    "arithmetic",
    "bubble_sort",
    "euclidean",
    "factorial",
    "fibonacci",
    "matrix_multiply"]
IPC_PATTERN = r"Misprediction Rate:\s*([\d.]+)%"


def run_make(btb_size):
    subprocess.run(
        f"make clean && make all OPTIONS=\"-DBTB_SIZE={btb_size}\"",
        shell=True,
        check=True,
        cwd=DIRECTORY)


def get_ipc(benchmark):
    print(f"Running {benchmark}")
    result = subprocess.run(
        [f"{DIRECTORY}/cpu", f"../benchmark_kernels/{benchmark}.bin"],
        stdout=subprocess.PIPE,
        text=True)
    match = re.search(IPC_PATTERN, result.stdout)
    misprediction_rate = float(match.group(1))
    print(f"Finished {benchmark} with misprediction_rate {misprediction_rate}")
    return misprediction_rate


def main():
    results = {
        benchmark: [] for benchmark in BENCHMARKS
    }

    for rob_size in range(1, 65):
        run_make(rob_size)
        with ThreadPoolExecutor() as executor:
            misprediction_result = executor.map(get_ipc, BENCHMARKS)
            for benchmark, misprediction_rate in zip(BENCHMARKS, misprediction_result):
                results[benchmark].append(misprediction_rate)
        print(results)


if __name__ == "__main__":
    main()
