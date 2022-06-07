.PHONY=clean all

CC = gcc

FLAGS = -Iinclude/ -fsanitize=address -g -pedantic -pedantic-errors -Wall

all: stegobmp

STEGO_OBJ = stegobmp.o utils/args.o

STEGO_C_FILES = stegobmp.c utils/args.c 

STEGO_H_FILES = include/utils/args.h
	
clean:	
	- rm -rf $(STEGO_OBJ) stegobmp

stegobmp: $(STEGO_OBJ) $(STEGO_H_FILES)
	$(CC) $(FLAGS) $(STEGO_OBJ) -o stegobmp
