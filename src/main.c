#include "simulation.h"

#ifdef MPI
#include <mpi.h>
#endif

int main(int argc, char** argv) {
	#ifdef MPI
		MPI_Init(&argc, &argv);

		int comm_sz;
		int rank;
		MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	#endif

	brownian_sim sim;

	if (argc != 5) {
		#ifdef MPI
			if (rank == ROOT_RANK) {
				printf("Usage: mpirun -np X %s <particles> <diff_coeff> <time_step> <end_time>\n", argv[0]);
			}
			MPI_Finalize();
		#else
			printf("Usage: %s <particles> <diff_coeff> <time_step> <end_time>\n", argv[0]);
		#endif
		return 1;
	}


	#ifdef MPI
		setup_simulation_mpi(&sim, argv, comm_sz, rank, MPI_COMM_WORLD);
	#else
		setup_simulation(&sim, argv);
	#endif

	brownian_results* results;
	results = brownian_run_simulation(sim);

	#ifdef MPI
		report_simulation_results_mpi(sim, results, comm_sz, rank, MPI_COMM_WORLD);
	#else
		report_simulation_results(sim, results);
	#endif

	brownian_free_results(results);

	#ifdef MPI
		MPI_Finalize();
	#endif

	return 0;
}

