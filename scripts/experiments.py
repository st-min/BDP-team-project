
import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('metrics.csv')
print(df.describe())

plt.figure()
df['cb_ms'].rolling(20).mean().plot()
plt.title('Callback time (ms, rolling mean)')
plt.xlabel('frame index')
plt.ylabel('ms')
plt.tight_layout()
plt.savefig('cb_ms.png')
print('Saved cb_ms.png')
