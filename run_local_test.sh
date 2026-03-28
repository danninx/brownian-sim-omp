#!/usr/bin/env bash
echo "Note: This script should be used from within a python virtual environment containing the necessary packages for data analysis"
echo "To setup, run:"
echo "`python -m venv .venv`"
echo "`source .venv/bin/activate`"
echo "`pip install pandas numpy matplotlib scipy`"

make

echo ""
echo "Running with 10,000 particles, D=1.0, time_step=0.001 seconds, simulated_time=100.000 seconds"
echo ""
echo "Running serial..."
./bin/brownian_serial 10000 1.0 0.01 100.0 > serial.csv
echo ""
echo "Running single-process multithreaded..."
./bin/brownian_omp 10000 1.0 0.01 100.0 > multithreaded.csv
echo ""
echo "Running single threaded MPI..."
./bin/brownian_mpi 10000 1.0 0.01 100.0 > mpi.csv
echo ""
echo "Running multithreaded MPI..."
./bin/brownian_mpi_omp 10000 1.0 0.01 100.0 > mpi_omp.csv

echo "Generating MSD vs. time graphs..."
python results.py serial.csv
python results.py multithreaded.csv
python results.py mpi.csv
python results.py mpi_omp.csv

