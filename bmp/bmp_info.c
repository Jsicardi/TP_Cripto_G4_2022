#include "../include/bmp/bmp_info.h"
#include "../include/utils/byte_utils.h"

bool io_op_to_bmp_info(BmpInfo *bmp_info, bool (*io_op)(void*, size_t, FILE*), FILE* file_descriptor){
    if(!io_op(&(bmp_info->biSize),          sizeof(bmp_info->biSize),          file_descriptor)) return false;
    if(!io_op(&(bmp_info->biWidth),         sizeof(bmp_info->biWidth),         file_descriptor)) return false;
    if(!io_op(&(bmp_info->biHeight),        sizeof(bmp_info->biHeight),        file_descriptor)) return false;
    if(!io_op(&(bmp_info->biPlanes),        sizeof(bmp_info->biPlanes),        file_descriptor)) return false;
    if(!io_op(&(bmp_info->biBitCount),      sizeof(bmp_info->biBitCount),      file_descriptor)) return false;
    if(!io_op(&(bmp_info->biCompression),   sizeof(bmp_info->biCompression),   file_descriptor)) return false;
    if(!io_op(&(bmp_info->biSizeImage),     sizeof(bmp_info->biSizeImage),     file_descriptor)) return false;
    if(!io_op(&(bmp_info->biXPelsPerMeter), sizeof(bmp_info->biXPelsPerMeter), file_descriptor)) return false;
    if(!io_op(&(bmp_info->biYPelsPerMeter), sizeof(bmp_info->biYPelsPerMeter), file_descriptor)) return false;
    if(!io_op(&(bmp_info->biClrUsed),       sizeof(bmp_info->biClrUsed),       file_descriptor)) return false;
    if(!io_op(&(bmp_info->biClrImportant),  sizeof(bmp_info->biClrImportant),  file_descriptor)) return false;
    return true;
}

bool read_bmp_info(BmpInfo *bmp_info, FILE* file_descriptor){
    return io_op_to_bmp_info(bmp_info, &read_x_bytes, file_descriptor);
}

bool write_bmp_info(BmpInfo *bmp_info, FILE* file_descriptor){
    return io_op_to_bmp_info(bmp_info, &write_x_bytes, file_descriptor);
}