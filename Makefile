.PHONY=clean all

CC = gcc

FLAGS = -Iinclude/ -fsanitize=address -g -pedantic -pedantic-errors -Wall

all: stegobmp

STEGO_OBJ = stegobmp.o args.o

STEGO_C_FILES = stegobmp.c args.c 

STEGO_H_FILES = include/args.h
	
clean:	
	- rm -rf $(STEGO_OBJ) stegobmp

stegobmp: $(STEGO_OBJ) $(STEGO_H_FILES)
	$(CC) $(FLAGS) $(STEGO_OBJ) -o stegobmp
