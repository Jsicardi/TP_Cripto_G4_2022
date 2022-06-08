#include "../include/utils/byte_utils.h"

#define LONE_BLOCK 1

bool read_x_bytes(void* buffer, size_t bytes_to_read, FILE* file_descriptor){
    size_t read_bytes = fread(buffer, bytes_to_read, LONE_BLOCK, file_descriptor);
    if(read_bytes != LONE_BLOCK)
        return false;
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

bool write_x_bytes(void* buffer, size_t bytes_to_write, FILE* file_descriptor){
    size_t written_bytes = fwrite(buffer, bytes_to_write, LONE_BLOCK, file_descriptor);
    if(written_bytes != LONE_BLOCK)
        return false;
    return true;
}