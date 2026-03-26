import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import linregress

df = pd.read_csv('output.csv')
df.columns = df.columns.str.strip()

time = df['Time']
msd = df['MSD']

D_theoretical = 1.0
msd_theoretical = 6.0 * D_theoretical * time

slope, intercept, r_value, p_value, std_err = linregress(time, msd)

D_simulated = slope / 6.0
error_margin = abs(D_theoretical - D_simulated) / D_theoretical * 100

plt.figure(figsize=(8, 6))

plt.plot(time, msd, label=f'Simulated MSD (D={D_simulated:.3f})', color='blue', linewidth=2)
plt.plot(time, msd_theoretical, label=f'Theoretical MSD (D={D_theoretical})', color='red', linestyle='--', linewidth=2)

plt.xlabel('Time (t)')
plt.ylabel('Mean Squared Displacement (MSD)')
plt.title(r'Brownian Motion Validation: MSD vs. Time ($\Delta t$ = 0.001)')
plt.legend()
plt.grid(True, linestyle=':', alpha=0.7)
plt.savefig('msd_validation_plot.png', dpi=300)
plt.show()
