#include "simulation.h"
#include <stdio.h>
#include <time.h>

int main() {
	brownian_sim sim;
	scanf("%ld", &sim.num_particles);
	scanf("%lf", &sim.diffusion_coefficient);
	scanf("%lf", &sim.time_step);
	scanf("%lf", &sim.end_time);
	sim.base_rng_seed = time(NULL);

	brownian_results* results;
	results = brownian_run_simulation(sim);

	printf("#%lf\n", results->elapsed);
	printf("Time, MSD\n");
	for (long i=0; i<results->iterations; i++) {
		printf("%lf, %Lf\n", i * sim.time_step, results->displacements[i]);	
	}

	brownian_free_results(results);

	return 0;
}
