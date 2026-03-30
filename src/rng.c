#include "rng.h"
#include <gsl/gsl_randist.h>

/*
 * Since GSL isn't installed on the cluster yet, trying out a drop-in replacement
 * using a thread-safe version of Sajjana's Box-Muller implementation;
 * I'm using the preprocessor to favor GSL's version whenever it's available
 * since GSL has mroe battle experience than our version
 */

#ifdef USE_GSL
//
// GSL version
// 
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

static gsl_rng** thread_rngs = NULL;

void rng_init_gsl(unsigned long base_seed, int max_threads, int rank) {
	gsl_rng_env_setup();
	const gsl_rng_type *rng_t = gsl_rng_default;

	thread_rngs = malloc(max_threads * sizeof(gsl_rng*));

	for (int i=0; i<max_threads; i++) {
		thread_rngs[i] = gsl_rng_alloc(rng_t);
		gsl_rng_set(thread_rngs[i], base_seed + (rank * max_threads) + i);
	}
}

double rng_gaussian_gsl(int tid, double sigma) {
	return gsl_ran_gaussian(thread_rngs[tid], sigma);
}

void rng_cleanup_gsl(int max_threads) {
	if (thread_rngs) {
		for (int i=0; i<max_threads; i++) {
			gsl_rng_free(thread_rngs[i]);
		}
		free(thread_rngs);
		thread_rngs = NULL;
	}
}

#else
//
// CUSTOM LOGIC
//
#include <threads.h>

static unsigned long global_base_seed = 0;
static int global_rank = 0;
static int global_max_threads = 1;

static thread_local unsigned int thread_seed;
static thread_local int seed_initialized = 0;

// setup thread-independent seed
void rng_init_thread(int tid) {
	thread_seed = (unsigned int) (global_base_seed + (global_rank * global_max_threads) + tid);
	seed_initialized = 1;
}

// setup the global RNG settings for this process
void rng_init_custom(unsigned long base_seed, int max_threads, int rank) {
	global_max_threads = max_threads;
	global_base_seed = base_seed;
	global_rank = rank;
}

// U ~ [0, 1]
double uniform_random(int tid) {
	if (!seed_initialized) {
		rng_init_thread(tid);
	}

	// use rand_r() with the thread seed over rand() for thread safety (see `man rand_r`)
	return (double) rand_r(&thread_seed) / RAND_MAX;
}

// thread safe Box-Muller with \sigma = sigma
// see https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform#Basic_form
double rng_gaussian_custom(int tid, double sigma) {
	double u1 = uniform_random(tid);
	double u2 = uniform_random(tid);

	if (u1 < 1e-10) {
		u1 = 1e-10;
	}

	double z0 = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
	return z0 * sigma;
}

// thread local and static global variables will be cleaned up on program exit, unlike the gsl_rng's which are malloc'd
void rng_cleanup_custom(int max_threads) {
	return;
}

#endif

//
// rng interface defines the three main functionalities we need for the simulation (at the moment anyhow)
// both the implementation and source codes are preprocessed based on whether GSL is compiled with the code
// by default it will use the custom implementations, but if you want to try it with GSL use the `make with_gsl`
// Makefile target
//
void rng_init(unsigned long base_seed, int max_threads, int rank) {
	#ifdef USE_GSL
		rng_init_gsl(base_seed, max_threads, rank);
	#else
		rng_init_custom(base_seed, max_threads, rank);
	#endif
}

double rng_gaussian(int tid, double sigma) {
	#ifdef USE_GSL
		return rng_gaussian_gsl(tid, sigma);
	#else
		return rng_gaussian_custom(tid, sigma);
	#endif
}

void rng_cleanup(int max_threads) {
	#ifdef USE_GSL
		rng_cleanup_gsl(max_threads);
	#else
		rng_cleanup_custom(max_threads);
	#endif

}

