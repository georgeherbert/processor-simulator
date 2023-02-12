CC = gcc
CFLAGS = -Wall -std=gnu11

all: assembler simple_processor

assembler: assembler.c rv32i.h
	$(CC) $(CFLAGS) -o assembler assembler.c

simple_processor: simple_processor.c rv32i.h
	$(CC) $(CFLAGS) -o simple_processor simple_processor.c

clean:
	rm -f assembler simple_processor
