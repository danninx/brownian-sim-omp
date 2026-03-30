RESULTS_FILE="test_timings.csv"
echo "JobId,ArrayId,Program,Nodes,Tasks,MaxThreads,Particles,DiffCoeff,TimeStep,EndTime,WallTime" > "$RESULTS_FILE"

echo "Submitting test jobs"
sbatch --nodes=1 --ntasks-per-node=1 --cpus-per-task=1 jobs/test_serial.sh
sbatch --nodes=1 --ntasks-per-node=1 --cpus-per-task=4 jobs/test_omp.sh
sbatch --nodes=1 --ntasks-per-node=4 --cpus-per-task=1 jobs/test_mpi.sh
sbatch --nodes=1 --ntasks-per-node=4 --cpus-per-task=4 jobs/test_mpi_omp.sh
