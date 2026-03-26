#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <time.h>

#define DIFFUSION_COEFF 1.0
#define DELTA_T 0.001
#define NUM_PARTICLES 10000

const double diffusion_coefficient = DIFFUSION_COEFF;
const double time_step = DELTA_T;

double step(gsl_rng *r) {
	return sqrt(2 * diffusion_coefficient * time_step) * gsl_ran_gaussian(r, 1.0);
}


typedef struct vec3d {
	double x;
	double y;
	double z;
} vec3d;

typedef struct particle {
	vec3d position;
} particle;

double squared_displacement(particle *p) {
	double x2 = (p->position.x * p->position.x);
	double y2 = (p->position.y * p->position.y);
	double z2 = (p->position.z * p->position.z);
	return x2 + y2 + z2;
}

void simulate_move(particle* p, gsl_rng *r) {
	p->position.x += step(r);
	p->position.y += step(r);
	p->position.z += step(r);
}

int main() {
	gsl_rng_env_setup();
	gsl_rng *r = gsl_rng_alloc(gsl_rng_mt19937);
	gsl_rng_set(r, time(NULL));

	particle *parts = malloc(NUM_PARTICLES * sizeof(particle));
	for (long i=0; i<NUM_PARTICLES; i++) {
		parts[i].position = (vec3d){0, 0, 0};
	}

	printf("Time, MSD\n");
	for (double t=0.0; t < 100; t += time_step) {
		long double mean_squared_displacement = 0;
		for (long j=0; j<NUM_PARTICLES; j++) {
			simulate_move(parts+j, r);
			long double r2 = squared_displacement(parts+j);
			mean_squared_displacement += r2;
		}
		printf("%lf, %Lf\n", t, (mean_squared_displacement / NUM_PARTICLES));
	}

	free(parts);
	gsl_rng_free(r);

	return 0;
}


