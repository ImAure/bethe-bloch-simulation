import numpy as np
import matplotlib.pyplot as plt
import sys

# Sostituisci 'dati.txt' con il nome del tuo file
filename = sys.argv[1]

# Carica i dati
distanza, energia1, energia2 = np.loadtxt(filename, unpack=True)

# Crea il grafico
plt.figure(figsize=(8, 5))
plt.plot(distanza, energia1 / np.max(energia1), label='Energia 1', color='blue')
plt.plot(distanza, energia2 / np.max(energia2), label='Energia 2', color='red')

# Etichette e titolo
plt.xlabel('Distanza')
plt.ylabel('Energia in %')
plt.title('Energia in funzione della distanza')
plt.legend()
plt.grid(True)

# Mostra il grafico
plt.tight_layout()
plt.show()
