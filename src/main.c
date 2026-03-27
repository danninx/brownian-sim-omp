#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#define NUM_PARTICLES 10000

typedef struct pos3d {
	double x;
	double y;
	double z;
} pos3d;

double square(double x) {
	return x * x;
}

int main() {
	gsl_rng_env_setup();
	const gsl_rng_type *rng_t = gsl_rng_default;
	const double diffusuion_coefficient = 1.0;
	const double time_step = 0.001;
	const double end_time = 10.0;
	const long num_particles = NUM_PARTICLES;
 
	int max_threads;
	#ifdef _OPENMP
		max_threads = omp_get_max_threads();
	#else
		max_threads = 1;
	#endif

	long NUM_ITERATIONS = 1000;
	double step_coefficient = sqrt(2 * diffusuion_coefficient * time_step);

	gsl_rng** thread_rngs = malloc(max_threads * sizeof(gsl_rng*));

	unsigned long base_seed = time(NULL);
	for (int i=0; i<max_threads; i++) {
		thread_rngs[i] = gsl_rng_alloc(rng_t);
		gsl_rng_set(thread_rngs[i], base_seed + i);
	}

	pos3d *starting_positions = malloc(num_particles * sizeof(pos3d));
	pos3d *current_positions = malloc(num_particles * sizeof(pos3d));
	long double *squared_displacements = calloc(NUM_ITERATIONS, sizeof(long double));

	#pragma omp parallel for
	for (size_t i=0; i<NUM_PARTICLES; i++) {
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


	#ifdef _OPENMP
		double start, end;
		start = omp_get_wtime();
	#else
		clock_t start, end;
		start = clock();
	#endif
	// simulation 
	#pragma omp parallel for reduction(+:squared_displacements[:NUM_ITERATIONS])
    for (long i = 0; i < num_particles; i++) {
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

        for (int iteration = 0; iteration < NUM_ITERATIONS; iteration++) {
            
            curr_x += step_coefficient * gsl_ran_gaussian(thread_rngs[tid], 1.0);
            curr_y += step_coefficient * gsl_ran_gaussian(thread_rngs[tid], 1.0);
            curr_z += step_coefficient * gsl_ran_gaussian(thread_rngs[tid], 1.0);
            
            squared_displacements[iteration] += (long double) (square(curr_x - start_x) + 
                                                               square(curr_y - start_y) + 
                                                               square(curr_z - start_z));
        }

        current_positions[i].x = curr_x;
        current_positions[i].y = curr_y;
        current_positions[i].z = curr_z;
    }

	#ifdef _OPENMP
		end = omp_get_wtime();
		double elapsed = end - start;
	#else
		end = clock();	
		double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
	#endif

	printf("Simulation completed in %f seconds.\n", elapsed);

	free(starting_positions);
	free(current_positions);
	free(squared_displacements);
	for (int i=0; i<max_threads; i++) {
		gsl_rng_free(thread_rngs[i]);
	}
	free(thread_rngs);

	return 0;
}
