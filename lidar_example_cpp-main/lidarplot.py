import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

# Définir le nom de fichier
MEAN = 'DataM/LidarMeanM0.txt'
BRUT = 'DataM/LidarClustM0.txt'
BEACON = 'DataM/LidarBalise0.txt'
BEACON1 = 'DataM/LidarBalise1.txt'


# Définir le motif de délimiteur basé sur l'expression régulière
#delimiteur_pattern = r'\s*theta: | Dist: | Q: '
delimiteur_pattern = r'angle : |, distance : '


# Lire le fichier texte avec pandas en utilisant le délimiteur personnalisé
brut = pd.read_csv(BRUT, sep=delimiteur_pattern, engine='python', header=None, names=['Theta', 'Dist'])
mean = pd.read_csv(MEAN, sep=delimiteur_pattern, engine='python', header=None, names=['Theta', 'Dist'])
beacon = pd.read_csv(BEACON, sep=delimiteur_pattern, engine='python', header=None, names=['Theta', 'Dist'])
beacon1 = pd.read_csv(BEACON1, sep=delimiteur_pattern, engine='python', header=None, names=['Theta', 'Dist'])


X_brut = np.nan_to_num(brut, nan=0)
print(brut)
X_mean = np.nan_to_num(mean, nan=0)
X_beacon = np.nan_to_num(beacon, nan=0)
X_beacon1 = np.nan_to_num(beacon1, nan=0)


# Convert to numpy arrays for easier manipulation
angles_brut = np.array(X_brut[:,0])
distances_brut = np.array(X_brut[:,1])

angles_mean = np.array(X_mean[:,0])
distances_mean = np.array(X_mean[:,1])

angles_beacon = np.array(X_beacon[:,0])
distances_beacon = np.array(X_beacon[:,1])

angles_beacon1 = np.array(X_beacon1[:,0])
distances_beacon1 = np.array(X_beacon1[:,1])



maxRad = 0.5

# Plotting the data on a polar plot
plt.figure(figsize=(8, 8))
ax = plt.subplot(111, projection='polar')
ax.scatter(np.radians(angles_brut), distances_brut,s=5)  # Convert angles to radians
ax.scatter(np.radians(angles_mean), distances_mean,s=20)  # Convert angles to radians
ax.scatter(np.radians(angles_beacon), distances_beacon,s=50, color='black')  # Convert angles to radians
ax.scatter(np.radians(angles_beacon1), distances_beacon1,s=50, color='blue')  # Convert angles to radians

ax.set_theta_zero_location('N')  # Set 0 degrees to North
ax.set_title('LiDAR Data on Polar Plot')
ax.set_rmax(np.ceil(distances_brut.max() / maxRad) * maxRad)  # Set the maximum radial value
ax.set_rticks(np.arange(0, ax.get_rmax() + maxRad, maxRad))  # Set radial ticks every 1000 units
plt.show()
