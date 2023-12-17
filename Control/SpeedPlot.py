import numpy as np
import matplotlib.pyplot as plt
import pandas as pd



# Lecture des données avec Pandas
data1 = pd.read_csv("Data/Speed2.txt", sep=',', header=None, decimal='.', names=['left', 'right'])

X1 = np.nan_to_num(data1, nan=0)



plt.figure()
plt.title("K1")
plt.plot(X1[:,0], label='left', linewidth=0.5)
plt.plot(X1[:,1], label='right', linewidth=.5)
plt.xlabel("t [s]")
plt.ylabel("Amplitude")
plt.legend()




plt.show()
