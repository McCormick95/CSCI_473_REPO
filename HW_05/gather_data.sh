#!/bin/bash

# Set the values of A, C, and H
A=0.5
C=1.0
H=1.0
u=1

# Set the number of processes for parallel execution
START_NUM=20000000
INCREMENT=20000000
MAX_NUM=40000000
# MAX_NUM=100000000
NUM_PROCESSES=4

# Function to extract values from the output
extract_values() {
    reflect=$(echo "$1" | grep "reflected" | awk -F'[(),=]' '{print $2}')
    absorb=$(echo "$1" | grep "absorbed" | awk -F'[(),=]' '{print $4}')
    transmit=$(echo "$1" | grep "transmitted" | awk -F'[(),=]' '{print $6}')
    prob_reflect=$(echo "$1" | grep "r/n" | awk '{print $3}')
    prob_absorb=$(echo "$1" | grep "a/n" | awk '{print $6}')
    prob_transmit=$(echo "$1" | grep "t/n" | awk '{print $9}')
    time=$(echo "$1" | grep "T:" | awk '{print $2}')
    echo "$reflect,$absorb,$transmit,$prob_reflect,$prob_absorb,$prob_transmit,$time"
}

echo "-------------RUNNING SERIAL-------------"
echo "n,serial_reflect,serial_absorb,serial_transmit,serial_prob_reflect,serial_prob_absorb,serial_prob_transmit,serial_time" > results_serial.csv
# Loop through different values of n
for (( n=START_NUM; n<=MAX_NUM; n+=INCREMENT )); do
    echo "Running simulations for n = $n"

    # Run serial version
    serial_output=$(./nt-serial -A $A -C $C -H $H -n $n -u $u)
    serial_values=$(extract_values "$serial_output")
    
    # Append results to CSV
    echo "$n,$serial_values" >> results_serial.csv
done

echo "------------RUNNING Parallel------------"
echo "n,np,parallel_reflect,parallel_absorb,parallel_transmit,parallel_prob_reflect,parallel_prob_absorb,parallel_prob_transmit,parallel_time" > results_parallel.csv
# Loop through different values of n
for (( n=START_NUM; n<=MAX_NUM; n+=INCREMENT )); do
    for (( np=1; np<=NUM_PROCESSES; np++ )); do
        echo "Running simulations for n = $n and np= $np"
    
        # Run parallel version
        parallel_output=$(mpirun -np $np ./nt-parallel -A $A -C $C -H $H -n $n -u $u)
        parallel_values=$(extract_values "$parallel_output")
        
        # Append results to CSV
        echo "$n,$np,$parallel_values" >> results_parallel.csv
    done
done

echo "Simulations complete. Results saved in results_serial.csv and results_parallel.csv"