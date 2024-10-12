#!/bin/bash
#SBATCH --job-name="rm_hw05"
#SBATCH --output="rm_hw05.%j.%N.out"
#SBATCH --partition=compute
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=128
#SBATCH --mem=16G
#SBATCH --account=ccu108
#SBATCH --export=ALL
#SBATCH -t 00:05:00

#This job runs with 1 nodes, 128 cores per node for a total of 128 tasks.

module load cpu/0.17.3b
module load gcc/10.2.0/npcyll4
module load openmpi/4.1.1

chmod +x gather_data.sh

bash gather_data.sh
