CC = gcc
MPICC = mpicc -DMPI
CFLAGS = -Wall -lm
GSL_FLAGS = -lgsl -lgslcblas
OPENMP_FLAGS = -fopenmp

BIN_DIR = bin

SERIAL_BIN = $(BIN_DIR)/brownian_serial
OPENMP_BIN = $(BIN_DIR)/brownian_omp
MPI_SINGLE_THREADED_BIN = $(BIN_DIR)/brownian_mpi
MPI_OMP_BIN = $(BIN_DIR)/brownian_mpi_omp

SRC = $(wildcard src/*.c)

.PHONY: all bin_dir  clean

all: CFLAGS += -O3
all: $(SERIAL_BIN) $(OPENMP_BIN) $(MPI_SINGLE_THREADED_BIN) $(MPI_OMP_BIN)

with_gsl: CFLAGS += $(GSL_FLAGS) -DUSE_GSL
with_gsl: all

bin_dir:
	@mkdir -p $(BIN_DIR)

clean:
	@rm -f $(SERIAL_BIN) $(OPENMP_BIN)

debug: CFLAGS += -g
debug: $(SERIAL_BIN) $(OPENMP_BIN) $(MPI_SINGLE_THREADED_BIN) $(MPI_OMP_BIN) 

$(SERIAL_BIN): bin_dir
	$(CC) $(CFLAGS) $(SRC) -Wno-unknown-pragmas -o $@

$(OPENMP_BIN): bin_dir
	$(CC) $(CFLAGS) $(OPENMP_FLAGS) $(SRC) -o $@

$(MPI_SINGLE_THREADED_BIN): bin_dir
	$(MPICC) $(CFLAGS) $(SRC) -Wno-unknown-pragmas -o $@

$(MPI_OMP_BIN): bin_dir
	$(MPICC) $(CFLAGS) $(OPENMP_FLAGS) $(SRC) -o $@

