#ifndef RNG_H
#define RNG_H

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

void rng_init(unsigned long base_seed, int max_threads, int rank);
double rng_gaussian(int tid, double sigma);
void rng_cleanup(int max_threads);

#endif
