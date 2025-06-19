# Bethe-Bloch Formula Simulation for Relativistic Charged Particles

This project provides a numerical implementation of the Bethe-Bloch formula to simulate the energy loss of relativistic charged particles (protons, alpha particles, or muons) as they travel through matter. The current implementation focuses on metal targets, specifically aluminum and copper.

This code was developed as part of a laboratory course in physics.

## Features

- Simulates energy loss of charged particles using the Bethe-Bloch equation  
- Supports different projectiles (proton, alpha particle, muon)  
- Supports aluminum and copper as absorber materials  
- Outputs data to a file and optionally plots the energy loss profile using Python  

## Compilation

To compile the simulation code, use:

```bash
gcc -o sim bethe-simulation.c -O3
```

This requires a working C compiler (such as `gcc`) on a Unix-like system.

## Usage

Once compiled, you can run the simulation with:

```bash
./sim <energy_in_MeV> <thickness_in_cm> <steps> <path/to/data>
```

The program will prompt you to select the projectile and target material. Once the simulation is complete, data will be written to the specified output file.  

You will also be asked whether you want to generate a plot of the energy loss. If you choose yes, make sure you have `python3` and the required Python libraries (e.g., `matplotlib`) installed.
