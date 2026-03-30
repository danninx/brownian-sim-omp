#include "simulation.h"

typedef struct pos3d {
	double x, y, z;
} pos3d;

double square(double x) {
	return x * x;
}

brownian_results* brownian_run_simulation(brownian_sim sim) {
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
	rng_init(time(NULL), max_threads, sim.rank);
	// setup starting particle positions
	#pragma omp parallel for
	for (size_t i=0; i<sim.num_particles; i++) {
		int tid;
		#ifdef _OPENMP
			tid = omp_get_thread_num();
		#else
			tid = 0;
		#endif 
		starting_positions[i].x = rng_gaussian(tid, 1.0);
		current_positions[i].x = starting_positions[i].x;
		starting_positions[i].y = rng_gaussian(tid, 1.0);
		current_positions[i].y = starting_positions[i].y;
		starting_positions[i].z = rng_gaussian(tid, 1.0);
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
	        
	        curr_x += motion_coefficient * rng_gaussian(tid, 1.0);
	        curr_y += motion_coefficient * rng_gaussian(tid, 1.0);
	        curr_z += motion_coefficient * rng_gaussian(tid, 1.0);
	        
	        displacements[iteration] += (long double) (square(curr_x - start_x) + square(curr_y - start_y) + square(curr_z - start_z));
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

	results->elapsed = elapsed;
	results->displacements = displacements;

	free(starting_positions);
	free(current_positions);
	rng_cleanup(max_threads);
	
	return results;
};

void setup_simulation(brownian_sim* sim, char** argv) {
	sim->num_particles = atol(argv[1]);
	sim->diffusion_coefficient = atof(argv[2]);
	sim->time_step = atof(argv[3]);
	sim->end_time = atof(argv[4]);

	sim->base_rng_seed = time(NULL);
	sim->rank = 0;
}

void report_simulation_results(brownian_sim sim, brownian_results* results) {
	// convert MSD's to means
	#pragma omp parallel for
	for (long i=0; i<results->iterations; i++) {
		results->displacements[i] /= sim.num_particles;
	}

	printf("#%lf\n", results->elapsed);
	printf("Time, MSD\n");
	for (long i=0; i<results->iterations; i++) {
		printf("%lf, %Lf\n", i * sim.time_step, results->displacements[i]);	
	}
}

#ifdef MPI
void setup_simulation_mpi(brownian_sim* sim, char** argv, int comm_sz, int rank, MPI_Comm comm) {
	if (rank == ROOT_RANK) {
		setup_simulation(sim, argv);
		sim->num_particles /= comm_sz;
	}	
	MPI_Bcast(&sim->num_particles, 1, MPI_LONG, ROOT_RANK, comm);
	MPI_Bcast(&sim->diffusion_coefficient, 1, MPI_DOUBLE, ROOT_RANK, comm);
	MPI_Bcast(&sim->time_step, 1, MPI_DOUBLE, ROOT_RANK, comm);
	MPI_Bcast(&sim->end_time, 1, MPI_DOUBLE, ROOT_RANK, comm);
	sim->base_rng_seed = time(NULL);
	sim->rank = rank;
}

void report_simulation_results_mpi(brownian_sim sim, brownian_results* results, int comm_sz, int rank, MPI_Comm comm) {
	long total_particles = sim.num_particles * comm_sz;			
	if (rank == ROOT_RANK) {
		MPI_Reduce(MPI_IN_PLACE, results->displacements, results->iterations, MPI_LONG_DOUBLE, MPI_SUM, ROOT_RANK, comm);
		MPI_Reduce(MPI_IN_PLACE, &results->elapsed, 1, MPI_DOUBLE, MPI_MAX, ROOT_RANK, comm);
		sim.num_particles = total_particles;
		report_simulation_results(sim, results);
	} else {
		MPI_Reduce(results->displacements, NULL, results->iterations, MPI_LONG_DOUBLE, MPI_SUM, ROOT_RANK, comm);
		MPI_Reduce(&results->elapsed, NULL, 1, MPI_DOUBLE, MPI_MAX, ROOT_RANK, comm);
	}
}
#endif
