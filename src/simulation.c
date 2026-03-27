#include "simulation.h"
#include <stdlib.h>
#include <time.h>

typedef struct pos3d {
	double x, y, z;
} pos3d;

double square(double x) {
	return x * x;
}

brownian_results* brownian_run_simulation(brownian_sim sim) {
	gsl_rng_env_setup();
	const gsl_rng_type *rng_t = gsl_rng_default;
	const long num_iterations = (long) sim.end_time / sim.time_step; 
	const double motion_coefficient = sqrt(2 * sim.diffusion_coefficient * sim.time_step);

	brownian_results* results;
	results = malloc(sizeof(brownian_results));
	results->iterations = num_iterations;
	long double *displacements = calloc(num_iterations, sizeof(long double));

	pos3d *starting_positions = calloc(sim.num_particles, sizeof(pos3d));
	pos3d *current_positions = calloc(sim.num_particles, sizeof(pos3d));

	int max_threads;
	#ifdef _OPENMP
		max_threads = omp_get_max_threads();
	#else
		max_threads = 1;
	#endif

	gsl_rng** thread_rngs = malloc(max_threads * sizeof(gsl_rng*));
	                                                                
	unsigned long base_seed = time(NULL);
	for (int i=0; i<max_threads; i++) {
		thread_rngs[i] = gsl_rng_alloc(rng_t);
		gsl_rng_set(thread_rngs[i], base_seed + i);
	}

	// setup starting particle positions
	#pragma omp parallel for
	for (size_t i=0; i<sim.num_particles; i++) {
		int tid;
		#ifdef _OPENMP
			tid = omp_get_thread_num();
		#else
			tid = 0;
		#endif 
		starting_positions[i].x = gsl_ran_gaussian(thread_rngs[tid], 1.0);
		current_positions[i].x = starting_positions[i].x;
		starting_positions[i].y = gsl_ran_gaussian(thread_rngs[tid], 1.0);
		current_positions[i].y = starting_positions[i].y;
		starting_positions[i].z = gsl_ran_gaussian(thread_rngs[tid], 1.0);
		current_positions[i].z = starting_positions[i].z;
	}

	// simulate the motion for each particle individually
	#ifdef _OPENMP
		double start, end;
		start = omp_get_wtime();
	#else
		clock_t start, end;
		start = clock();
	#endif

	#pragma omp parallel for reduction(+:displacements[:num_iterations])
	for (long i = 0; i < sim.num_particles; i++) {
	    int tid;
	 #ifdef _OPENMP
	 	tid = omp_get_thread_num();
	 #else
	 	tid = 0;
	 #endif
	    
	    double start_x = starting_positions[i].x;
	    double start_y = starting_positions[i].y;
	    double start_z = starting_positions[i].z;
	    
	    double curr_x = start_x;
	    double curr_y = start_y;
	    double curr_z = start_z;
	
	    for (int iteration = 0; iteration < num_iterations; iteration++) {
	        
	        curr_x += motion_coefficient * gsl_ran_gaussian(thread_rngs[tid], 1.0);
	        curr_y += motion_coefficient * gsl_ran_gaussian(thread_rngs[tid], 1.0);
	        curr_z += motion_coefficient * gsl_ran_gaussian(thread_rngs[tid], 1.0);
	        
	        displacements[iteration] += (long double) (square(curr_x - start_x) + square(curr_y - start_y) + square(curr_z - start_z));
	    }
	
	    current_positions[i].x = curr_x;
	    current_positions[i].y = curr_y;
	    current_positions[i].z = curr_z;
	}

	// convert MSD's to means
	#pragma omp parallel for
	for (long i=0; i<num_iterations; i++) {
		displacements[i] /= sim.num_particles;
	}

	#ifdef _OPENMP
		end = omp_get_wtime();
		double elapsed = end - start;
	#else
		end = clock();	
		double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
	#endif

	results->elapsed = elapsed;
	results->displacements = displacements;

	free(starting_positions);
	free(current_positions);
	for (int i=0; i<max_threads; i++) {
		gsl_rng_free(thread_rngs[i]);
	}
	free(thread_rngs);
	
	return results;
};

