#!/usr/bin/env python3

from os.path import isfile
import numpy as np
import matplotlib.pyplot as plt
import sys
import os

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Argomenti insufficienti") 
        exit(-1)
    filename = sys.argv[1]
    if not os.path.isfile(filename):
        print(f"File {filename} non esiste.")
        exit(-1)

    # Carica i dati
    try: 
        distanza, energia1, energia2 = np.loadtxt(filename, unpack=True)
    except ValueError:
        print("Il file non contiene dati validi o formattati correttamente.")
        exit(-1)

    # Crea il grafico
    plt.figure(figsize=(8, 5))
    plt.plot(distanza, energia1 / np.max(energia1), label="Energia 1", color="blue")
    plt.plot(distanza, energia2 / np.max(energia2), label="Energia 2", color="red")

    # Etichette e titolo
    plt.xlabel("Distanza")
    plt.ylabel("Energia in %")
    plt.title("Energia in funzione della distanza")
    plt.legend()
    plt.grid(True)

    # Mostra il grafico
    plt.tight_layout()
    plt.show()
