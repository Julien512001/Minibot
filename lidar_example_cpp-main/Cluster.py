import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from sklearn.cluster import AgglomerativeClustering
import plotly.express as px


# Définir le nom de fichier
nom_fichier = 'Data/LidarDataBeacon.txt'

# Définir le motif de délimiteur basé sur l'expression régulière
#delimiteur_pattern = r'\s*theta: | Dist: | Q: '
delimiteur_pattern = r'angle : |, distance : '

# Lire le fichier texte avec pandas en utilisant le délimiteur personnalisé
donnees = pd.read_csv(nom_fichier, sep=delimiteur_pattern, engine='python', header=None, names=['Theta', 'Dist'])

X = np.nan_to_num(donnees, nan=0)
print(np.shape(X))

X = X[X[:, 1] != 0]
print(np.shape(X))

# Convert to numpy arrays for easier manipulation
angles = np.array(X[:,0])
distances = np.array(X[:,1])

x = np.array([np.cos(np.pi*angles/180) * distances, np.sin(np.pi*angles/180) * distances]).T



cluster = AgglomerativeClustering().fit(x)

label = cluster.fit_predict(x)
print(label)


'''
def distance(x1, x2):
    dist = np.sqrt((x2[1]-x1[0])**2 + (x1[1]-x1[0])**2)

    return dist

cluster = np.zeros((len(x), 2))


for i in range(len(x)-1):
    for j in range(3):
        cluster[i] = x[i]
        if (distance(x[i],x[i+1]) < 0.5):
            cluster[i+1] = x[i+1]
        else:
            j += 1

'''