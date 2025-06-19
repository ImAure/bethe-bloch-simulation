#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define STRLEN  1024
#define ARGNUM  4       /* ./executable <initial_energy> <thickness> <steps> <output_file> */
#define MAX_ATTEMPTS 3

#define ERR         -1
#define ERR_ARGC    1
#define ERR_FILE    2

#define E_M      0.511   /* mass of electron in MeV / c^2 */
#define K       0.307   /* constant K in Bethe--Bloch's formula */

#define ALUMINUM 1
#define AL_Z 13
#define AL_A 27
#define AL_R 2.7
#define AL_I 166.0 /* eV */

#define COPPER 2
#define CU_Z 29
#define CU_A 63
#define CU_R 8.96
#define CU_I 322.0 /* eV */

#define PROTON 1
#define PROTON_Z 1
#define PROTON_M 938.272 /* MeV/c^2 */

#define ALPHA 2
#define ALPHA_Z 2
#define ALPHA_M 3727.379 /* MeV/c^2 */

#define MUON 3
#define MUON_Z 1
#define MUON_M 105.658 /* MeV/c^2 */

#define MAX(A,B)        (A > B) ? A : B

typedef struct material {
    int id;
    unsigned z;     /* atomic number */
    unsigned a;     /* atomic mass */
    double r;       /* density in g/cm^3 */
    double i;       /* mean excitation energy in eV */
} material_t;

typedef struct particle {
    int id;
    unsigned z;     /* charge */
    double m;       /* mass in MeV / c^2 */
    double t;       /* kinetic energy in MeV */
} particle_t;

typedef FILE file_t;

int myexit(const int status, const char *message) {
    switch (status) {
        case ERR_ARGC:
            fprintf(stderr, "! Error: Incorrect number of arguments.\n");
            fprintf(stderr, "! Usage: %s <initial_energy(MeV)> <thickness(cm)> <steps> <output_file>\n", message);
            return status;
        case ERR_FILE:
            fprintf(stderr, "! Error: Could not open output file at '%s'.\n", message);
            return status;
        default:
            fprintf(stderr, "! Error: An error occurred.\n");
            if (message) fprintf(stderr, "! Message: %s\n", message);
            fprintf(stderr, "! Exiting with code: %d\n", status);
            return status;
    }
}

int input_target_material(material_t *ptr_material) {
    int choice;
    char buffer[STRLEN];

    fprintf(stdout, "Choose target material:\n");
    fprintf(stdout, "1. Aluminum\n");
    fprintf(stdout, "2. Copper\n");
    fprintf(stdout, "Enter choice (1 or 2): ");

    if (fgets(buffer, STRLEN, stdin) == NULL) return 1;
    choice = atoi(buffer); 

    switch (choice) {
        case ALUMINUM:
            ptr_material->id = ALUMINUM;
            ptr_material->z = AL_Z;
            ptr_material->a = AL_A;
            ptr_material->r = AL_R;
            ptr_material->i = AL_I;
            return 0;
        case COPPER:
            ptr_material->id = COPPER;
            ptr_material->z = CU_Z;
            ptr_material->a = CU_A;
            ptr_material->r = CU_R;
            ptr_material->i = CU_I;
            return 0;
        default:
            return 1;
    }
}

int input_projectile(particle_t *ptr_projectile) {
    int choice;
    char buffer[STRLEN];

    fprintf(stdout, "Choose projectile:\n");
    fprintf(stdout, "1. Proton\n");
    fprintf(stdout, "2. Alpha particle\n");
    fprintf(stdout, "3. Muon\n");
    fprintf(stdout, "Enter choice (1, 2 or 3): ");

    if (fgets(buffer, STRLEN, stdin) == NULL) return 1;
    choice = atoi(buffer); 

    switch (choice) {
        case PROTON:
            ptr_projectile->id = PROTON;
            ptr_projectile->z = PROTON_Z;
            ptr_projectile->m = PROTON_M;
            return 0;
        case ALPHA:
            ptr_projectile->id = ALPHA;
            ptr_projectile->z = ALPHA_Z;
            ptr_projectile->m = ALPHA_M;
            return 0;
        case MUON:
            ptr_projectile->id = MUON;
            ptr_projectile->z = MUON_Z;
            ptr_projectile->m = MUON_M;
            return 0;
        default:
            return 1;
    }
}

int yes_or_no(char *buffer) {
    if (buffer == NULL) return 0;
    if (buffer[0] == 'y' || buffer[0] == 'Y') return 1;
    return 0;
}

double wmax(double beta2, double gamma, double gamma2, double m, double m2) {
    return (2.0 * E_M * beta2 * gamma2) / (1.0 + (2.0 * gamma * m) + m2);
}

double bethe(double z_inc, double z_tar, double a_tar, double beta2, double gamma2, double w_max, double i2) {
    return (((K * z_inc * z_inc * z_tar) / (a_tar * beta2)) * (0.5 * log((2.0 * E_M * beta2 * gamma2 * w_max) / (i2 * 1.0e-12)) - beta2));
}

int main(int argc, char *argv[]) {
    int         attempts;
    double      step, distance, kinetic_energy, total_energy, stopping_power;
    double      gamma, gamma_sq, beta_sq, m, m_sq, i_sq;
    char        buffer[STRLEN];
    file_t     *ptr_file_out;
    material_t  target;
    particle_t  projectile;

    if (argc != ARGNUM + 1) return myexit(ERR_ARGC, argv[0]);

    const double  initial_energy = atof(argv[1]);
    const double  thickness      = atof(argv[2]);
    const int     total_steps    = atoi(argv[3]);
    ptr_file_out                 = fopen(argv[4], "w");    
    
    if (initial_energy <= 0)    return myexit(ERR, "Kinetic energy must be positive.");
    if (total_steps    <= 0)    return myexit(ERR, "Number of steps must be positive.");
    if (thickness      <= 0)    return myexit(ERR, "Thickness must be positive.");
    if (ptr_file_out   == NULL) return myexit(ERR_FILE, argv[4]);
    
    for (attempts = 0; attempts < MAX_ATTEMPTS; attempts++) {
        if (attempts >= 1) fprintf(stdout, "! Try again. Attempt: %d/%d\n", attempts + 1, MAX_ATTEMPTS);
        if (!input_target_material(&target)) break;
        if (attempts == (MAX_ATTEMPTS - 1)) return myexit(ERR, "Too many attempts at input material.");
    }

    for (attempts = 0; attempts < MAX_ATTEMPTS; attempts++) {
        if (attempts >= 1) fprintf(stdout, "! Try again. Attempt: %d/%d\n", attempts + 1, MAX_ATTEMPTS);
        if (!input_projectile(&projectile)) break;
        if (attempts == (MAX_ATTEMPTS - 1)) return myexit(ERR, "Too many attempts at input projectile.");
    }

    step = thickness / total_steps;
    m    = E_M / projectile.m;
    m_sq = m * m;
    i_sq = target.i * target.i;

    for (kinetic_energy = initial_energy; (distance < thickness); distance += step) {
        total_energy = kinetic_energy + projectile.m;
        gamma    = total_energy / projectile.m;
        gamma_sq = gamma * gamma;
        beta_sq = 1. - (1. / gamma_sq);

        stopping_power = MAX(0., (target.r * bethe(projectile.z, target.z, target.a, beta_sq, gamma_sq, wmax(beta_sq, gamma, gamma_sq, m, m_sq), i_sq)));

        fprintf(ptr_file_out, "%.8f %.8f %.8f", distance, kinetic_energy, stopping_power);
        if (distance < thickness) fprintf(ptr_file_out, "\n");
        kinetic_energy = MAX(0, kinetic_energy - (stopping_power * step));
    }

    fclose(ptr_file_out);
    fprintf(stdout, "Data saved at '%s'\n", argv[4]);
    fprintf(stdout, "Do you wish to see a python plot? (y/n): ");
    if (yes_or_no(fgets(buffer, STRLEN, stdin))) {
        sprintf(buffer, "python3 plottino.py %s", argv[4]);
        system(buffer);
    }
    return 0;
}