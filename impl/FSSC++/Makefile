#Run this makefile to compile examples/example.cpp and tests/test.cpp
PREFIX = /usr/local
dbg = -g
CC=g++
CFLAGS=-fomit-frame-pointer -maes -std=c++11 -msse4.2 -fno-strict-aliasing -pedantic -Wall -Wextra -Wunreachable-code -Wmissing-declarations -Wunused-function -Wno-overlength-strings -Wno-deprecated-declarations -O3 -I ../src/
LDFLAGS= -L/usr/lib/x86_64-linux-gnu -Wl,-Bstatic -lcrypto -Wl,-Bdynamic -ldl -lssl -lgmpxx -lgmp -lpthread -I$GOOGLETESTDIR/include -L$GOOGLETESTDIR/lib -lgtest -lbenchmark -I src/
EX_SOURCES=  src/fss-client.cpp src/fss-server.cpp src/prg.cpp examples/example.cpp
TEST_SOURCES= src/fss-client.cpp src/fss-server.cpp src/prg.cpp tests/test.cpp
BENCHMARK_SOURCES = src/fss-client.cpp src/fss-server.cpp src/prg.cpp eval/benchmark.cpp
OBJ_DIR = obj
EX_OBJECTS=$(EX_SOURCES:src/%.cpp=$(OBJ_DIR)/%.o)
TEST_OBJECTS=$(TEST_SOURCES:src/%.cpp=$(OBJ_DIR)/%.o)
BENCHMARK_OBJ=$(BENCHMARK_SOURCES:src/%.cpp=$(OBJ_DIR)/%.o)

EX_EXECUTABLE=examples/example
TEST_EXECUTABLE=tests/test
BENCHMARK_EXECTUABLE = eval/benchmark

ifeq ($(AESNI), 1)
    CFLAGS+=-DAESNI
endif

all: $(EX_EXECUTABLE) $(TEST_EXECUTABLE) $(BENCHMARK_EXECTUABLE)

$(EX_EXECUTABLE): $(EX_OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(TEST_EXECUTABLE): $(TEST_OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(BENCHMARK_EXECTUABLE): $(BENCHMARK_OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: src/%.cpp | $(OBJ_DIR)
	$(CC) $(CFLAGS) $< -c -o $@

$(OBJ_DIR):
	mkdir -p $@

.PHONY: clean all
clean:
	rm -f *.o $(EX_EXECUTABLE) $(TEST_EXECUTABLE)