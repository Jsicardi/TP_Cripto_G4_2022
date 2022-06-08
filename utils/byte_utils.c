#include "../include/utils/byte_utils.h"

#define LONE_BLOCK 1

bool read_x_bytes(void* buffer, size_t bytes_to_read, FILE* file_descriptor){
    size_t read_bytes = fread(buffer, bytes_to_read, LONE_BLOCK, file_descriptor);
    if(read_bytes != LONE_BLOCK)
        return false;
    return true;
}

bool drop_x_bytes(size_t bytes_to_drop, FILE* file_descriptor){
    size_t read_character;
    while(bytes_to_drop > 0){
        read_character = fgetc(file_descriptor);
        if(read_character == EOF)
            return false;
        bytes_to_drop--;
    }
    return true;
}