.PHONY=clean all

CC = gcc

FLAGS = -Iinclude/ -fsanitize=address -g -pedantic -pedantic-errors -Wall

all: stegobmp

STEGO_OBJ = stegobmp.o utils/args.o utils/byte_utils.o bmp/bmp_header.o bmp/bmp_info.o bmp/bmp_file.o

STEGO_C_FILES = stegobmp.c utils/args.c utils/byte_utils.c bmp/bmp_header.c bmp/bmp_info.c bmp/bmp_file.c

STEGO_H_FILES = include/utils/args.h include/utils/byte_utils.h include/bmp/bmp_header.h include/bmp/bmp_info.h include/bmp/bmp_file.h
	
clean:	
	- rm -rf $(STEGO_OBJ) stegobmp

stegobmp: $(STEGO_OBJ) $(STEGO_H_FILES)
	$(CC) $(FLAGS) $(STEGO_OBJ) -o stegobmp
