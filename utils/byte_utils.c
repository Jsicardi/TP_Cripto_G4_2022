#include "../include/utils/byte_utils.h"

#include <stdlib.h>
#include <string.h>

#define LONE_BLOCK 1

bool io_op_to_x_bytes(  void* buffer, 
                        size_t bytes_to_op, 
                        size_t (*io_op)(void *, size_t, size_t, FILE *), 
                        FILE* file_descriptor
                    ){
    size_t resulting_bytes = io_op(buffer, bytes_to_op, LONE_BLOCK, file_descriptor);
    if(resulting_bytes != LONE_BLOCK)
        return false;
    return true;
}

bool read_x_bytes(void* buffer, size_t bytes_to_read, FILE* file_descriptor){
    return io_op_to_x_bytes(buffer, bytes_to_read, &fread, file_descriptor);
}

bool write_x_bytes(void* buffer, size_t bytes_to_write, FILE* file_descriptor){
    
    /* 
        fwrite has to be cast to (size_t (*)(void *, size_t, size_t, FILE *)) as 
        it is of type (size_t (*)(const void *, size_t, size_t, FILE *)) pay attention
        to the const which should correspond to the buffer from which it reads.
    */

    return io_op_to_x_bytes(buffer, bytes_to_write, (size_t (*)(void *, size_t, size_t, FILE *)) &fwrite, file_descriptor);
}

bool prepend_x_bytes(uint8_t ** buffer, size_t buffer_size, size_t bytes_to_write, void *buffer_to_write){
    
    *buffer = realloc(*buffer, buffer_size + bytes_to_write);
    if(*buffer == NULL) return false;
    
    memmove(*buffer + bytes_to_write, *buffer, buffer_size);
    memcpy(*buffer, buffer_to_write, bytes_to_write);

    return true;
}

bool dump_x_bytes(size_t bytes_to_dump, FILE* origin_fd){
    size_t read_character;
    while(bytes_to_dump > 0){
        read_character = fgetc(origin_fd);
        if(read_character == EOF)
            return false;
        bytes_to_dump--;
    }
    return true;
}

bool copy_x_bytes(size_t bytes_to_copy, FILE* origin_fd, FILE* destination_fd){
    size_t read_character;
    while(bytes_to_copy > 0){
        read_character = fgetc(origin_fd);
        if(read_character == EOF)
            return false;
        if(fputc(read_character, destination_fd) == EOF) return false;
        bytes_to_copy--;
    }
    return true;
}