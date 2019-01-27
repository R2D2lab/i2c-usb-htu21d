CC=gcc
SRC = $(wildcard *.c)
NAME = HTU21D

all: $(SRC)
	$(CC) $^ -o $(NAME)
clean:
	rm -f $(NAME)
