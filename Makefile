NAME=dns
SRC=$(NAME).c
BIN=$(NAME)

CC=clang
CFLAGS=-Wall -Wextra -pedantic

all: $(BIN)

clean:
	rm $(BIN) || true

$(BIN): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^


