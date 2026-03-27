gcc main.c -fopenmp -lgsl -lgslcblas -lm -o omp.out
gcc main.c -lgsl -lgslcblas -lm -o serial.out
