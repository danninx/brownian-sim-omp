#!/bin/bash
#SBATCH --job-name=brownian_test_mpi_omp_%A
#SBATCH --output=logs/test_mpi_omp_%A.out
#SBATCH --time=02:00:00

# ---- JUST CHANGE THESE ----
BIN="bin/brownian_mpi_omp"
NAME="mpi_omp"
# ---------------------------

# ---------------------------
module purge
module load gnu15/15.1.0
module load openmpi5/5.0.8
module load prun/2.2
# ---------------------------

PARAMS="100000 1.5 0.001 100.0"
export OMP_NUM_THREADS=${SLURM_CPUS_PER_TASK:-1}

# time the program
START=$(date +%s.%N)
prun $BIN $PARAMS
END=$(date +%s.%N)
WALL_TIME=$(awk -v t1="$START" -v t2="$END" 'BEGIN {printf "%.8f", t2-t1}')

# create headers if the file isn't already there
RESULTS_FILE="test_timings.csv"
echo "${SLURM_ARRAY_JOB_ID},${SLURM_ARRAY_TASK_ID},${NAME},${SLURM_JOB_NUM_NODES:-1},${SLURM_NTASKS:-1},${SLURM_CPUS_PER_TASK:-1},${PARAMS// /,},${WALL_TIME}" >> "$RESULTS_FILE"
