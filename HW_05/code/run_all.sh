#!/bin/bash

make clean all

cd ../data/

# SWEEP DATA
python3 gather_sweep_data.py demo_data
python3 plot_sweep_data.py demo_data

# PERFORMANCE DATA
# python3 gather_performance_data.py demo_data_2
# python3 gather_performance_data.py demo_data_2



