#include "../include/bmp/bmp_file.h"
#include "../include/utils/byte_utils.h"

/***** PRIVATE FUNCTIONS *****/

/*
    Given a BMP File which had its metadata (BmpHeader and BmpInfo) read, it removes the trailing bytes previous to
    the BMP File body. The expected result is the file_descriptor pointing to the beginning of the body bytes.

    It should be executed just once per file descriptor having met the conditions above described.
 */

bool remove_offset_bytes_from_bmp(BmpFile * bmp_file, FILE * file_descriptor);

bool remove_offset_bytes_from_bmp(BmpFile * bmp_file, FILE * file_descriptor){
    size_t bytes_to_remove = (bmp_file->header).bfOffBits - BMP_HEADER_SIZE - BMP_INFO_SIZE;
    if(bytes_to_remove < 0)
        return false;
    else if(bytes_to_remove > 0)
        if(!drop_x_bytes(bytes_to_remove, file_descriptor)) return false;
    return true;
}

/***** PRIVATE FUNCTIONS *****/

/***** PUBLIC FUNCTIONS *****/

bool read_bmp_file_metadata(BmpFile * bmp_file, FILE * file_descriptor){
    if(!read_bmp_header(&(bmp_file->header), file_descriptor))   return false;
    if(!read_bmp_info(&(bmp_file->info), file_descriptor))       return false;
    if(!remove_offset_bytes_from_bmp(bmp_file, file_descriptor)) return false;
    return true;
}

bool read_bmp_file_pixel(Pixel * pixel, FILE * file_descriptor){
    return read_x_bytes(pixel, PIXEL_SIZE, file_descriptor);
}

/***** PUBLIC FUNCTIONS *****/