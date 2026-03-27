#ifndef BROWNIAN_MOTION_SIMULATION
#define BROWNIAN_MOTION_SIMULATION

#include <stdlib.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <math.h>

#ifdef _OPENMP
#include <omp.h>
#else
#include <time.h>
#endif

typedef struct brownian_sim {
	double diffusion_coefficient;
	double time_step;
	double end_time;
	long num_particles;
	unsigned long base_rng_seed;
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

#endif
