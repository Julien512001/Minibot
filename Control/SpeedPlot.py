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
