PREFIX = /usr/local
dbg = -g
CC=g++
CFLAGS=-fomit-frame-pointer -maes -std=c++11 -msse4.2 -fno-strict-aliasing -pedantic -Wall -Wextra -Wunreachable-code -Wmissing-declarations -Wunused-function -Wno-overlength-strings -Wno-deprecated-declarations -O3 
LDFLAGS= -L/usr/lib/x86_64-linux-gnu -Wl,-Bstatic -lcrypto -Wl,-Bdynamic -ldl -lssl -lgmpxx -lgmp -lpthread -I$GOOGLETESTDIR/include -L$GOOGLETESTDIR/lib -lgtest
EX_SOURCES=  fss-client.cpp fss-server.cpp prg.cpp example.cpp
TEST_SOURCES= fss-client.cpp fss-server.cpp prg.cpp test.cpp
EX_OBJECTS=$(EX_SOURCES:.cpp=.o)
TEST_OBJECTS=$(TEST_SOURCES:.cpp=.o)

EX_EXECUTABLE=example
TEST_EXECUTABLE=test

ifeq ($(AESNI), 1)
    CFLAGS+=-DAESNI
endif

all: $(EX_EXECUTABLE) $(TEST_EXECUTABLE)

$(EX_EXECUTABLE): $(EX_OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(TEST_EXECUTABLE): $(TEST_OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -c -o $@

.PHONY: clean all
clean:
	rm -f *.o $(EX_EXECUTABLE) $(TEST_EXECUTABLE)