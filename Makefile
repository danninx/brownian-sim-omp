CC = gcc
CFLAGS = -Wall -O3 -lm
GSL_FLAGS = -lgsl -lgslcblas
OPENMP_FLAGS = -fopenmp

BIN_DIR = bin

SERIAL_BIN = $(BIN_DIR)/brownian_serial
OPENMP_BIN = $(BIN_DIR)/brownian_omp

SRC = $(wildcard src/*.c)

.PHONY: all bin_dir  clean

all: $(SERIAL_BIN) $(OPENMP_BIN)

bin_dir:
	@mkdir -p $(BIN_DIR)

clean:
	@rm -f $(SERIAL_BIN) $(OPENMP_BIN)

$(SERIAL_BIN): bin_dir
	$(CC) $(CFLAGS) $(GSL_FLAGS) $(SRC) -Wno-unknown-pragmas -o $@

$(OPENMP_BIN): bin_dir
	$(CC) $(CFLAGS) $(GSL_FLAGS) $(OPENMP_FLAGS) $(SRC) -o $@

