RESULTS_FILE="master_timings.csv"
echo "JobId,ArrayId,Program,Nodes,Tasks,MaxThreads,Particles,DiffCoeff,TimeStep,EndTime,WallTime" > "$RESULTS_FILE"

#JOBS=( jobs/* )
NUM_THREADS=( 1 2 4 8 16 32 )
NUM_TASKS=( 1 2 4 8 16 32 )
NUM_NODES=( 1 2 4 8 )
CPUS_PER_NODE=32

# serial need only one node
echo "Submitting Serial jobs..."
sbatch jobs/serial.sh

# omp only variable in threads
echo "Submitting OpenMP jobs..."
for threads in "${NUM_THREADS[@]}"; do
	sbatch --nodes=1 --ntasks-per-node=1 --cpus-per-task=$threads jobs/omp.sh
done

# mpi only variable in nodes/tasks
echo "Submitting MPI jobs..."
for nodes in "${NUM_NODES[@]}"; do
	for tasks in "${NUM_TASKS[@]}"; do
		sbatch --nodes=$nodes --ntasks-per-node=$tasks --cpus-per-task=1 jobs/mpi.sh
	done
done

# hybrid is variable in both
echo "Submitting hybrid jobs..."
for nodes in "${NUM_NODES[@]}"; do
	for tasks in "${NUM_TASKS[@]}"; do
		for threads in "${NUM_THREADS[@]}"; do
			# make sure the job isn't requesting more CPUs than our nodes have
			if [ $((tasks * threads)) -le $CPUS_PER_NODE ]; then
				sbatch --nodes=$nodes --ntasks-per-node=$tasks --cpus-per-task=$threads jobs/mpi_omp.sh
			fi
		done
	done
done

echo "\nJobs submitted!"
