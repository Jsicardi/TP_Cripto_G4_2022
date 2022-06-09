#include "../include/bmp/bmp_header.h"
#include "../include/utils/byte_utils.h"

bool io_op_to_bmp_header(BmpHeader *bmp_header, bool (*io_op)(void*, size_t, FILE*), FILE* file_descriptor) {
    if(!io_op(&(bmp_header->bfType),      sizeof(bmp_header->bfType),      file_descriptor)) return false;
    if(!io_op(&(bmp_header->bfSize),      sizeof(bmp_header->bfSize),      file_descriptor)) return false;
    if(!io_op(&(bmp_header->bfReserved1), sizeof(bmp_header->bfReserved1), file_descriptor)) return false;
    if(!io_op(&(bmp_header->bfReserved2), sizeof(bmp_header->bfReserved2), file_descriptor)) return false;
    if(!io_op(&(bmp_header->bfOffBits),   sizeof(bmp_header->bfOffBits),   file_descriptor)) return false;
    return true;
}

void clean_bmp_header(BmpHeader *bmp_header){
    bmp_header->bfType      = 0;
    bmp_header->bfSize      = 0;
    bmp_header->bfReserved1 = 0;
    bmp_header->bfReserved2 = 0;
    bmp_header->bfOffBits   = 0;
}

bool read_bmp_header(BmpHeader *bmp_header, FILE* file_descriptor){
    return io_op_to_bmp_header(bmp_header, &read_x_bytes, file_descriptor);
}

bool write_bmp_header(BmpHeader *bmp_header, FILE* file_descriptor){
    return io_op_to_bmp_header(bmp_header, &write_x_bytes, file_descriptor);
}

void get_bmp_file_body_pixel_count(uint32_t * pixel_count, BmpHeader * bmp_header){
    *pixel_count = (bmp_header->bfSize - bmp_header->bfOffBits)/3;
}