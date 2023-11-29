'''
import numpy as np 
import matplotlib.pyplot as plt

    # Lecture du fichier et conversion des lignes en entiers
with open("Speed.txt", 'r') as file:
    content = file.readlines()
    integers_list = [int(line.strip()) for line in content]

# Conversion de la liste d'entiers en une NumPy array
list = np.array(integers_list)
t = np.linspace(0,10,len(list))

file.close()
print(list)

plt.plot(t,list)
plt.show()
'''
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd



# Lecture des données avec Pandas
data1 = pd.read_csv("Control/Speed.txt", sep=',', header=None, decimal='.', names=['left', 'right'])

X1 = np.nan_to_num(data1, nan=0)



plt.figure()
plt.title("K1")
plt.plot(X1[:,0], label='left', linewidth=0.5)
plt.plot(X1[:,1], label='right', linewidth=.5)
plt.xlabel("t [s]")
plt.ylabel("Amplitude")
plt.legend()




plt.show()
