import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

# Définir le nom de fichier
nom_fichier = 'Data/LidarDataTable0.txt'

# Définir le motif de délimiteur basé sur l'expression régulière
#delimiteur_pattern = r'\s*theta: | Dist: | Q: '
delimiteur_pattern = r'angle : |, distance : '

# Lire le fichier texte avec pandas en utilisant le délimiteur personnalisé
donnees = pd.read_csv(nom_fichier, sep=delimiteur_pattern, engine='python', header=None, names=['Theta', 'Dist'])

X = np.nan_to_num(donnees, nan=0)
print(donnees)

# Convert to numpy arrays for easier manipulation
angles = np.array(X[:,0])
distances = np.array(X[:,1])




maxRad = 0.5

# Plotting the data on a polar plot
plt.figure(figsize=(8, 8))
ax = plt.subplot(111, projection='polar')
ax.scatter(np.radians(angles), distances,s=5)  # Convert angles to radians
ax.set_theta_zero_location('N')  # Set 0 degrees to North
ax.set_title('LiDAR Data on Polar Plot')
ax.set_rmax(np.ceil(distances.max() / maxRad) * maxRad)  # Set the maximum radial value
ax.set_rticks(np.arange(0, ax.get_rmax() + maxRad, maxRad))  # Set radial ticks every 1000 units
plt.show()
