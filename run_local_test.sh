#!/usr/bin/env bash
echo "Note: This script should be used from within a python virtual environment containing the necessary packages for data analysis"
echo "To setup, run:"
echo "`python -m venv .venv`"
echo "`source .venv/bin/activate`"
echo "`pip install pandas numpy matplotlib scipy`"

make
mkdir -p data

echo ""
echo "Running with 10,000 particles, D=1.0, time_step=0.001 seconds, simulated_time=100.000 seconds"
PARAMS="10000 1.0 0.001 100.0"

echo ""
echo "Running serial..."
./bin/brownian_serial $PARAMS > data/serial.csv
echo ""
echo "Running single-process multithreaded..."
./bin/brownian_omp $PARAMS > data/multithreaded.csv
echo ""
echo "Running single threaded MPI..."
mpirun -np 4 ./bin/brownian_mpi $PARAMS > data/mpi.csv
echo ""
echo "Running multithreaded MPI..."
mpirun -np 4 ./bin/brownian_mpi_omp $PARAMS > data/mpi_omp.csv

echo "Generating MSD vs. time graphs..."
python results.py data/serial.csv
python results.py data/multithreaded.csv
python results.py data/mpi.csv
python results.py data/mpi_omp.csv

