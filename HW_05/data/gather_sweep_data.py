import subprocess
import os
import sys
import argparse

def run_make_clean_all():
    try:
        cwd_string = "../code/"
        subprocess.run(["make", "clean", "all"], cwd=cwd_string, check=True)
        print("Make clean all completed successfully.")
    except subprocess.CalledProcessError as e:
        print(f"Error during make clean all: {e}")
        exit(1)

def run_c_program(arg1, arg2):
    try:
        cwd_string = "../code/"
        result = subprocess.run(["./nt-serial", "-C", "10.0", "-A", f"{arg1}", "-H", f"{arg2}", "-n", "1000", "-u", "1"], 
                                cwd=cwd_string, capture_output=True, text=True, check=True)
        return result.stdout.strip().split('\n')[1]
    except subprocess.CalledProcessError as e:
        print(f"Error running C program: {e}")
        return None
    except IndexError:
        print("Unexpected output format from C program")
        return None

def float_range(start, stop, step):
    while start < stop:
        yield round(start, 2)
        start += step

def main(output_file):
    C=10.00
    n=1000
    run_make_clean_all()

    with open(output_file, "w") as output_file:
        output_file.write("C,A,H,n,time,refl,abs,tran,rn,an,tn,\n")

        for A in float_range(0.01, 0.51, 0.01):  # Outer loop: 0.01 to 0.50, step 0.01
            for H in float_range(0.01, 10.00, 0.01):  # Inner loop: 0.01 to 10.00, step 0.01
                output = run_c_program(A, H)
                if output:
                    output_file.write(f"{C:.2f},{A:.2f},{H:.2f},{n},{output}\n")
                    print(f"Ran C program with args {A:.2f}, {H:.2f}")

    print("Execution completed. Results saved in {output_file}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Run C program and save output to a CSV file.")
    parser.add_argument("output_file", help="Name of the output CSV file")
    args = parser.parse_args()

    main(args.output_file)