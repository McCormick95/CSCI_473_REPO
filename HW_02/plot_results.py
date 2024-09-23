import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

data = pd.read_csv('expanse_compute_results.txt')
font_size = 15

N_1 = data [['P', 'T']][data['N'] == 150000000]
N_2 = data [['P', 'T']][data['N'] == 250000000]
N_3 = data [['P', 'T']][data['N'] == 350000000]
N_4 = data [['P', 'T']][data['N'] == 450000000]
N_5 = data [['P', 'T']][data['N'] == 550000000]

plt.title("TIME vs PROCESS COUNT", fontsize = font_size)
plt.xlabel("PROCESS COUNT (P)", fontsize = font_size)
plt.ylabel("TIME (T)", fontsize = font_size)

IDEAL_T = N_1["T"].values[0] / N_1['P']

plt.plot(N_1['P'], N_1['T'], label = '150M', marker='o')
plt.plot(N_2['P'], N_2['T'], label = '250M', marker='x')
plt.plot(N_3['P'], N_3['T'], label = '350M', marker='v')
plt.plot(N_4['P'], N_4['T'], label = '450M', marker='*')
plt.plot(N_5['P'], N_5['T'], label = '550M', marker='+')
plt.plot(N_1['P'], IDEAL_T, label= "IDEAL", marker = '.', linestyle ='dotted', color = 'black')
plt.legend(fontsize = font_size)

plt.xticks(fontsize = font_size)
plt.yticks(fontsize = font_size)

plt.savefig('T_VS_P.png', bbox_inches='tight')
print('TIME vs PROCESS COUNT saved as T_VS_P.png')
# plt.show()