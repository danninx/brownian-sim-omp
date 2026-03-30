#ifndef BROWNIAN_MOTION_SIMULATION
#define BROWNIAN_MOTION_SIMULATION

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "rng.h"

#ifdef MPI
#include <mpi.h>
#endif
#define ROOT_RANK 0

#ifdef _OPENMP
#include <omp.h>
#else
#include <time.h>
#endif

#define BASE_SEED 1024

typedef struct brownian_sim {
	double diffusion_coefficient;
	double time_step;
	double end_time;
	long num_particles;
	unsigned long base_rng_seed;
	int rank;
} brownian_sim;

typedef struct brownian_results {
	double elapsed;
	long iterations;
	long double* displacements;
} brownian_results;

#define brownian_free_results(results) {\
	free(results->displacements); \
	free(results); \
}

brownian_results* brownian_run_simulation(brownian_sim sim);

void setup_simulation(brownian_sim* sim, char** argv);
void report_simulation_results(brownian_sim sim, brownian_results* results); 

#ifdef MPI
void setup_simulation_mpi(brownian_sim* sim, char** argv, int comm_sz, int rank, MPI_Comm comm);
void report_simulation_results_mpi(brownian_sim sim, brownian_results* results, int comm_sz, int rank, MPI_Comm comm);
#endif

#endif
