CC = gcc
EXEC = build/aster
CFLAGS = -Wall -Wextra -Werror
SRCS = $(shell find src -name '*.c')

all:
	mkdir -p build
	$(CC) $(CFLAGS) -o $(EXEC) $(SRCS)

run:
	make all
	./$(EXEC) $(ARGS)

clean:
	rm -rf build