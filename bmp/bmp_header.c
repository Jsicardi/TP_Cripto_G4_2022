#include "../include/bmp/bmp_header.h"
#include "../include/utils/byte_utils.h"

bool read_bmp_header(BmpHeader *bmp_header, FILE* file_descriptor){
    if(!read_x_bytes(&(bmp_header->bfType),      sizeof(bmp_header->bfType),      file_descriptor)) return false;
    if(!read_x_bytes(&(bmp_header->bfSize),      sizeof(bmp_header->bfSize),      file_descriptor)) return false;
    if(!read_x_bytes(&(bmp_header->bfReserved1), sizeof(bmp_header->bfReserved1), file_descriptor)) return false;
    if(!read_x_bytes(&(bmp_header->bfReserved2), sizeof(bmp_header->bfReserved2), file_descriptor)) return false;
    if(!read_x_bytes(&(bmp_header->bfOffBits),   sizeof(bmp_header->bfOffBits),   file_descriptor)) return false;
    return true;
}