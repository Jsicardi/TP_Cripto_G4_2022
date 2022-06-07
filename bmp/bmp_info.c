#include "../include/bmp/bmp_info.h"
#include "../include/utils/byte_utils.h"

bool read_bmp_info(BmpInfo *bmp_info, FILE* file_descriptor){
    if(!read_x_bytes(&(bmp_info->biSize),          sizeof(bmp_info->biSize),          file_descriptor)) return false;
    if(!read_x_bytes(&(bmp_info->biWidth),         sizeof(bmp_info->biWidth),         file_descriptor)) return false;
    if(!read_x_bytes(&(bmp_info->biHeight),        sizeof(bmp_info->biHeight),        file_descriptor)) return false;
    if(!read_x_bytes(&(bmp_info->biPlanes),        sizeof(bmp_info->biPlanes),        file_descriptor)) return false;
    if(!read_x_bytes(&(bmp_info->biBitCount),      sizeof(bmp_info->biBitCount),      file_descriptor)) return false;
    if(!read_x_bytes(&(bmp_info->biCompression),   sizeof(bmp_info->biCompression),   file_descriptor)) return false;
    if(!read_x_bytes(&(bmp_info->biSizeImage),     sizeof(bmp_info->biSizeImage),     file_descriptor)) return false;
    if(!read_x_bytes(&(bmp_info->biXPelsPerMeter), sizeof(bmp_info->biXPelsPerMeter), file_descriptor)) return false;
    if(!read_x_bytes(&(bmp_info->biYPelsPerMeter), sizeof(bmp_info->biYPelsPerMeter), file_descriptor)) return false;
    if(!read_x_bytes(&(bmp_info->biClrUsed),       sizeof(bmp_info->biClrUsed),       file_descriptor)) return false;
    if(!read_x_bytes(&(bmp_info->biClrImportant),  sizeof(bmp_info->biClrImportant),  file_descriptor)) return false;
    return true;
}