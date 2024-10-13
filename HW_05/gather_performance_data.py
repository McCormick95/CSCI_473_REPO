import subprocess
import csv

def make_clean_all():
    subprocess.run(["make", "clean", "all"], check=True)

# Set the values of A, C, and H
A = 0.5
C = 1.0
H = 1.0
u = 1

# Set the number of processes for parallel execution
START_NUM = 25000000
INCREMENT = 50000000
MAX_NUM = 250000000
NUM_PROCESSES = 64

def extract_values(output):
    return output.strip().split('\n')[-1]

def run_serial(n):
    cmd = ["./nt-serial", "-A", str(A), "-C", str(C), "-H", str(H), "-n", str(n), "-u", str(u)]
    result = subprocess.run(cmd, capture_output=True, text=True, check=True)
    return extract_values(result.stdout)

def run_parallel(n, np):
    cmd = ["mpirun", "-oversubscribe", "-np", str(np), "./nt-parallel", 
           "-A", str(A), "-C", str(C), "-H", str(H), "-n", str(n), "-u", str(u)]
    result = subprocess.run(cmd, capture_output=True, text=True, check=True)
    return extract_values(result.stdout)

def main():
    make_clean_all()

    print("-------------RUNNING SERIAL-------------")
    with open('results_serial.csv', 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(["n", "serial_reflect", "serial_absorb", "serial_transmit", 
                         "serial_prob_reflect", "serial_prob_absorb", "serial_prob_transmit", "serial_time"])
        
        for n in range(START_NUM, MAX_NUM + 1, INCREMENT):
            print(f"Running simulations for n = {n}")
            serial_values = run_serial(n)
            writer.writerow([n] + serial_values.split(','))

    print("------------RUNNING Parallel------------")
    with open('results_parallel.csv', 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(["n", "np", "time", "parallel_reflect", "parallel_absorb", "parallel_transmit", 
                         "parallel_prob_reflect", "parallel_prob_absorb", "parallel_prob_transmit"])
        
        for n in range(START_NUM, MAX_NUM + 1, INCREMENT):
            for np in range(1, NUM_PROCESSES + 1):
                print(f"Running simulations for n = {n} and np = {np}")
                parallel_values = run_parallel(n, np)
                writer.writerow([n, np] + parallel_values.split(','))

    print("Simulations complete. Results saved in results_serial.csv and results_parallel.csv")

if __name__ == "__main__":
    main()