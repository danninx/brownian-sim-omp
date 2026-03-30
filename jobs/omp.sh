#!/bin/bash
#SBATCH --job-name=brownian_test_serial_%a
#SBATCH --output=logs/serial_%A_%a.out
#SBATCH --array=1-6
#SBATCH --time=02:00:00

# ---- JUST CHANGE THESE ----
BIN="bin/brownian_omp"
NAME="omp"
# ---------------------------

# ---------------------------
module purge
module load gnu15/15.1.0
module load openmpi5/5.0.8
module load prun/2.2
# ---------------------------

PARAMS=$(sed -n "${SLURM_ARRAY_TASK_ID}p" inputs.txt)
export OMP_NUM_THREADS=${SLURM_CPUS_PER_TASK:-1}

# time the program
START=$(date +%s.%N)
prun $BIN $PARAMS > /dev/null
END=$(date +%s.%N)
WALL_TIME=$(awk -v t1="$START" -v t2="$END" 'BEGIN {printf "%.3f", t2-t1}')

# create headers if the file isn't already there
RESULTS_FILE="master_timings.csv"
if [ ! -f "$RESULTS_FILE" ]; then
    echo "JobId,ArrayId,Program,Nodes,Tasks,MaxThreads,Particles,DiffCoeff,TimeStep,EndTime,WallTime" > "$RESULTS_FILE"
fi
echo "${SLURM_ARRAY_JOB_ID},${SLURM_ARRAY_TASK_ID},${NAME},${SLURM_JOB_NUM_NODES:-1},${SLURM_NTASKS:-1},${SLURM_CPUS_PER_TASK:-1},${PARAMS// /,},${WALL_TIME}" >> "$RESULTS_FILE"
