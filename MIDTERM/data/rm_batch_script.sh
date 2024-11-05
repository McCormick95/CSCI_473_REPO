#!/bin/bash
#SBATCH --job-name="rm_midterm"
#SBATCH --output="rm_midterm.%j.%N.out"
#SBATCH --mail-user=rlmccormi@coastal.edu
#SBATCH --mail-type=BEGIN,END
#SBATCH --partition=debug
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=128
#SBATCH --mem=60G
#SBATCH --account=ccu108
#SBATCH --export=ALL
#SBATCH -t 00:30:00

#This job runs with 1 nodes, 128 cores per node for a total of 128 tasks.

module load cpu/0.17.3b
module load gcc/10.2.0/npcyll4

chmod +x gather_data.sh

bash gather_data.sh
