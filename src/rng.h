#ifndef RNG_H
#define RNG_H

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

//
// rng interface (see source for more details)
//

// initialize rng parameters for this process
void rng_init(unsigned long base_seed, int max_threads, int rank);

// gaussian RNG using thread_id (for seeding) and sigma
double rng_gaussian(int tid, double sigma);

// cleanup rng memory (if needed)
void rng_cleanup(int max_threads);

#endif
