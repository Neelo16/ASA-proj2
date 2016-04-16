CC=gcc
CFLAGS=-O3 -ansi -Wall -g
LIBS=-lm
BIN=./shortest
SOURCE=shortest.c

all: $(BIN)

$(BIN): $(SOURCE)
	$(CC) -o $(BIN) $(CFLAGS) $(SOURCE) $(LIBS)

clean:
	rm -f $(BIN)

run: $(BIN)
	$(BIN)

test:
	@./runtests.sh
