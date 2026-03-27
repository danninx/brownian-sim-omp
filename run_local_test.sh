#!/usr/bin/env bash
echo "Note: This script should be used from within a python virtual environment containing the necessary packages for data analysis"
echo "To setup, run:"
echo "`python -m venv .venv`"
echo "`source .venv/bin/activate`"
echo "`pip install pandas numpy matplotlib scipy`"

make

echo ""
echo "Running with 100,000 particles, D=1.0, time_step=0.001 seconds, simulated_time=100.000 seconds"
echo ""
echo "Running serial..."
echo "100000 1.0 0.001 100.0" | ./bin/brownian_serial > serial.csv
echo ""
echo "Running multithreaded..."
echo "100000 1.0 0.001 100.0" | ./bin/brownian_omp > multithreaded.csv 

echo "Generating MSD vs. time graphs..."
python results.py serial.csv
python results.py multithreaded.csv

