import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# data = pd.read_csv('expanse_compute_results.txt')
data = pd.read_csv('results_parallel_expanse.txt')
font_size = 15

N_1 = data [['np', 'time']][data['n'] == 25000000]
N_2 = data [['np', 'time']][data['n'] == 50000000]
N_3 = data [['np', 'time']][data['n'] == 75000000]
N_4 = data [['np', 'time']][data['n'] == 100000000]
N_5 = data [['np', 'time']][data['n'] == 125000000]
N_6 = data [['np', 'time']][data['n'] == 150000000]
N_7 = data [['np', 'time']][data['n'] == 175000000]
N_8 = data [['np', 'time']][data['n'] == 200000000]
N_9 = data [['np', 'time']][data['n'] == 225000000]
N_10 = data [['np', 'time']][data['n'] == 250000000]



# TIME VS PROCESS COUNT
plt.title("TIME vs PROCESS COUNT", fontsize = font_size)
plt.xlabel("PROCESS COUNT (P)", fontsize = font_size)
plt.ylabel("TIME (S)", fontsize = font_size)

IDEAL_T = N_1["time"].values[0] / N_1['np']

plt.plot(N_1['np'], N_1['time'], label = '25M')
plt.plot(N_2['np'], N_2['time'], label = '50M')
plt.plot(N_3['np'], N_3['time'], label = '75M')
plt.plot(N_4['np'], N_4['time'], label = '100M')
plt.plot(N_5['np'], N_5['time'], label = '125M')
plt.plot(N_6['np'], N_6['time'], label = '150M')
plt.plot(N_7['np'], N_7['time'], label = '175M')
plt.plot(N_8['np'], N_8['time'], label = '200M')
plt.plot(N_9['np'], N_9['time'], label = '225M')
plt.plot(N_10['np'], N_10['time'], label = '250M')
plt.plot(N_1['np'], IDEAL_T, label= "IDEAL", marker = '.', linestyle ='dotted', color = 'black')
plt.legend(fontsize = font_size)

plt.xticks(fontsize = font_size)
plt.yticks(fontsize = font_size)

plt.savefig('timing_plot.png', bbox_inches='tight')
print('TIME vs PROCESS COUNT saved as timing_plot.png')
# plt.show()