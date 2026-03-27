# brownian-sim
some physics thing for HPC

planning on using MPI, but wanted to try out OpenMP for multithreading

## Compiling
The `Makefile` is setup to produce two versions of the program for testing:
1. `bin/brownian_serial` is a single threaded version of the program
2. `bin/brownian_omp` uses OpenMP

## Testing vs. Theoretical Values
The python script can analyze the mean-squared displacements produced in the simulations over time:
```sh
python -m venv .venv

# this command is likely different on windows
source .venv/bin/activate

pip install numpy pandas scipy matplotlib
```

Once packages are installed you can just use the run script to run both
```sh
chmod +x ./run_local_test.sh
./run_local_test.sh
```

As a heads up the serial version takes a while to run on large inputs


