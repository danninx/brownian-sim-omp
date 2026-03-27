#include "simulation.h"
#include <mpi.h>

int main() {
	brownian_sim sim;

	#ifdef MPI
		MPI_Init(NULL, NULL);

		int comm_sz;
		int rank;
		MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);

		setup_simulation_mpi(&sim, comm_sz, rank, MPI_COMM_WORLD);
	#else
		setup_simulation(&sim);
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

