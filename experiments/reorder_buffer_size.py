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
IPC_PATTERN = r"IPC:\s+([\d.]+)"


def run_make(rob_size):
    subprocess.run(
        f"make clean && make all OPTIONS=\"-DREORDER_BUFFER_SIZE={rob_size}\"",
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
    ipc = float(match.group(1))
    print(f"Finished {benchmark} with IPC {ipc}")
    return ipc


def main():
    results = {
        benchmark: [] for benchmark in BENCHMARKS
    }

    for rob_size in range(1, 65):
        run_make(rob_size)
        with ThreadPoolExecutor() as executor:
            ipc_results = executor.map(get_ipc, BENCHMARKS)
            for benchmark, ipc in zip(BENCHMARKS, ipc_results):
                results[benchmark].append(ipc)
        print(results)
        # ipc = get_ipc("factorial")
        # print(f"IPC: {ipc}")


if __name__ == "__main__":
    main()
