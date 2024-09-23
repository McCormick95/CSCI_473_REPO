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


# TIME VS PROCESS COUNT
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

plt.savefig('timing_550000000_16.png', bbox_inches='tight')
print('TIME vs PROCESS COUNT saved as timing_550000000_16.png')
# plt.show()



# SPEEDUP VS PROCESS COUNT
# clears the figure
plt.clf()

N_all = [N_1, N_2, N_3, N_4, N_5]
for df in N_all:
     T_1 = df[df['P'] == 1]['T'].values[0]
     df['SPEEDUP'] = T_1 / df['T']

plt.title("SPEEDUP VS PROCESS COUNT", fontsize = font_size)
plt.xlabel("PROCESS COUNT (P)", fontsize = font_size)
plt.ylabel("SPEEDUP", fontsize = font_size)

plt.plot(N_1['P'], N_1['SPEEDUP'], label = '150M', marker='o')
plt.plot(N_2['P'], N_2['SPEEDUP'], label = '250M', marker='x')
plt.plot(N_3['P'], N_3['SPEEDUP'], label = '350M', marker='v')
plt.plot(N_4['P'], N_4['SPEEDUP'], label = '450M', marker='*')
plt.plot(N_5['P'], N_5['SPEEDUP'], label = '550M', marker='+')
plt.plot(N_1['P'], N_1['P'], label= "IDEAL", marker = '.', linestyle ='dotted', color = 'black')
plt.legend(fontsize = font_size)

plt.xticks(fontsize = font_size)
plt.yticks(fontsize = font_size)

plt.savefig('speedup_550000000_16.png', bbox_inches='tight')
print('SPEEDUP VS PROCESS COUNT saved as speedup_550000000_16.png')



# EFFICIENCY VS PROCESS COUNT
# clears the figure
plt.clf()

for df in N_all:
    df['EFFICIENCY'] = df['SPEEDUP']/df['P']

plt.title("EFFICIENCY VS PROCESS COUNT", fontsize = font_size)
plt.xlabel("PROCESS COUNT (P)", fontsize = font_size)
plt.ylabel("EFFICIENCY", fontsize = font_size)

plt.plot(N_1['P'], N_1['EFFICIENCY'], label = '150M', marker='o')
plt.plot(N_2['P'], N_2['EFFICIENCY'], label = '250M', marker='x')
plt.plot(N_3['P'], N_3['EFFICIENCY'], label = '350M', marker='v')
plt.plot(N_4['P'], N_4['EFFICIENCY'], label = '450M', marker='*')
plt.plot(N_5['P'], N_5['EFFICIENCY'], label = '550M', marker='+')
plt.plot(N_1['P'], [1]*len(N_1['P']), label= "IDEAL", marker = '.', linestyle ='dotted', color = 'black')
plt.legend(fontsize = font_size)

plt.xticks(fontsize = font_size)
plt.yticks(fontsize = font_size)

plt.savefig('efficiency_550000000_16.png', bbox_inches='tight')
print('EFFICIENCY VS PROCESS COUNT saved as efficiency_550000000_16.png')