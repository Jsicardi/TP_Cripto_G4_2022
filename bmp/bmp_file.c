#include "../include/bmp/bmp_file.h"
#include "../include/utils/byte_utils.h"
#include "../include/stego/stego.h"

/***** PRIVATE FUNCTIONS *****/



/*---- META DATA OPS ----*/

/*
    Given a bmp_file structure and a file_descritptor, applies an operation to the bmp_file header 
    and another to the bmp_file info.
*/

bool io_op_to_bmp_file_metadata(BmpFile * bmp_file, 
                                bool (*io_op_to_header) (BmpHeader *bmp_header, FILE* file_descriptor), 
                                bool (*io_op_to_info)   (BmpInfo *bmp_info, FILE* file_descriptor), 
                                FILE * file_descriptor);

bool io_op_to_bmp_file_metadata(BmpFile * bmp_file, 
                                bool (*io_op_to_header) (BmpHeader *bmp_header, FILE* file_descriptor), 
                                bool (*io_op_to_info)   (BmpInfo *bmp_info, FILE* file_descriptor), 
                                FILE * file_descriptor){
    if(!io_op_to_header(&(bmp_file->header), file_descriptor))   return false;
    if(!io_op_to_info(&(bmp_file->info), file_descriptor))       return false;
    return true;
}

/*---- META DATA OPS ----*/


/*---- BODY OPS ----*/

/*
    Sets all BmpBody fields to 0 to prevent leaking information into memory
    and becoming a potential security vulnerability.
 */

void clean_bmp_file_body(BmpFile * bmp_file);

void clean_bmp_file_body(BmpFile * bmp_file){
    (bmp_file->body).pixel_count = 0;
}

/*---- BODY OPS ----*/



/***** PRIVATE FUNCTIONS *****/





/***** PUBLIC FUNCTIONS *****/



void clean_bmp_file_structure(BmpFile * bmp_file){
    clean_bmp_header(&(bmp_file->header));
    clean_bmp_info(&(bmp_file->info));
    clean_bmp_file_body(bmp_file);
}



/*---- META DATA OPS ----*/

bool read_bmp_file_metadata(BmpFile * bmp_file, bool (*is_valid_metadata)(BmpFile*), FILE * file_descriptor){
    if(!io_op_to_bmp_file_metadata(bmp_file, &read_bmp_header, &read_bmp_info, file_descriptor)) return false;
    if(!is_valid_metadata(bmp_file)) return false;

    /* Sets bmp_file body structure size to be the file size - the metadata offset */
    get_bmp_file_body_pixel_count(&((bmp_file->body).pixel_count), &(bmp_file->header));

    return true;
}

bool write_bmp_file_metadata(BmpFile * bmp_file, FILE * file_descriptor){
    return io_op_to_bmp_file_metadata(bmp_file, &write_bmp_header, &write_bmp_info, file_descriptor);
}

bool copy_bmp_file_metadata(BmpFile * bmp_file, bool (*is_valid_metadata)(BmpFile*), FILE * origin_fd, FILE * destination_fd){
    if(!read_bmp_file_metadata(bmp_file, is_valid_metadata, origin_fd)) return false;
    if(!write_bmp_file_metadata(bmp_file, destination_fd))              return false;
    return true;
}

bool bmp_file_is_compressed(BmpFile * bmp_file){
    if((bmp_file->info).biCompression == 0)
        return false;
    return true;
}

bool bmp_file_is_uncompressed(BmpFile * bmp_file){
    return !bmp_file_is_compressed(bmp_file);
}

/*---- META DATA OPS ----*/



/*---- OFFSET OPS ----*/

bool copy_bmp_file_offset(BmpFile * bmp_file, FILE * origin_fd, FILE * destination_fd){
    size_t offset_bytes = (bmp_file->header).bfOffBits - BMP_HEADER_SIZE - BMP_INFO_SIZE;
    if(offset_bytes < 0)
        return false;
    else if(offset_bytes > 0)
        if(!copy_x_bytes(offset_bytes, origin_fd, destination_fd)) return false;
    return true;
}

/*---- OFFSET OPS ----*/



/*---- PIXEL OPS ----*/

bool read_bmp_file_pixel(Pixel * pixel, FILE * file_descriptor){
    return read_x_bytes(pixel, PIXEL_SIZE, file_descriptor);
}

bool write_bmp_file_pixel(Pixel * pixel, FILE * file_descriptor){
    return write_x_bytes(pixel, PIXEL_SIZE, file_descriptor);
}

bool transform_bmp_file_pixel(bool (*transformation) (Pixel*, BinaryMessage *,int), BinaryMessage * msg, FILE * origin_fd, FILE * destination_fd,int first_low_bit_position){
    Pixel pixel;
    if(!read_bmp_file_pixel(&pixel, origin_fd))       return false;
    if(!transformation(&pixel, msg,first_low_bit_position))                  return false;
    if(!write_bmp_file_pixel(&pixel, destination_fd)) return false;

    /* For security reasons we reset the Pixel values after the transfer */
    pixel.blue  = 0;
    pixel.green = 0;
    pixel.red   = 0;

    return true;
}

/*---- PIXEL OPS ----*/

/*---- BODY OPS ----*/

bool transform_bmp_file_body(BmpFile * bmp_file, bool (*transformation) (Pixel*, BinaryMessage *,int), BinaryMessage * msg, FILE * origin_fd, FILE * destination_fd,int first_low_bit_position){
    for(uint32_t i = 0; i < (bmp_file->body).pixel_count; i++){
        if(!transform_bmp_file_pixel(transformation, msg, origin_fd, destination_fd,first_low_bit_position)) return false;
    }
    return true;
}

bool transform_bmp_file_body_lsb1(BmpFile *bmp_file,bool (*transformation) (Pixel*, BinaryMessage *,int),BinaryMessage * msg, FILE * origin_fd, FILE * destination_fd) {
    return transform_bmp_file_body(bmp_file, &insert_lsb_pixel, msg, origin_fd, destination_fd, FIST_LOW_BIT_POSITION_LSB1);
}

bool transform_bmp_file_body_lsb4(BmpFile *bmp_file,bool (*transformation) (Pixel*, BinaryMessage *,int),BinaryMessage * msg, FILE * origin_fd, FILE * destination_fd){
    return transform_bmp_file_body(bmp_file, &insert_lsb_pixel, msg, origin_fd, destination_fd,FIST_LOW_BIT_POSITION_LSB4);
}

bool transform_bmp_file_body_lsbi(BmpFile *bmp_file,bool (*transformation) (Pixel*, BinaryMessage *,int),BinaryMessage * msg, FILE * origin_fd, FILE * destination_fd){

    uint8_t (*pattern_functions[4]) (uint8_t bit);
    uint8_t inversion_byte = 0;
    get_inversion_functions(bmp_file->body,msg,origin_fd,pattern_functions,&inversion_byte);
    invert_lsbi_message_bits(bmp_file->body,msg,origin_fd,pattern_functions);

    uint8_t * msg_with_inv = malloc(1 + (msg->last_byte_ptr - msg->curr_byte_ptr + 1));
    memcpy(msg_with_inv,&inversion_byte, sizeof(inversion_byte));
    memcpy(msg_with_inv + sizeof(inversion_byte),msg->curr_byte_ptr,(msg->last_byte_ptr - msg->curr_byte_ptr + 1));

    free(msg->message);
    load_binary_message(msg_with_inv,msg_with_inv + (1 + (msg->last_byte_ptr - msg->curr_byte_ptr + 1)),msg);
    
    //skip 4 superior bits of inversion byte
    uint8_t discard_bit_ptr;
    for(uint32_t i =0; i < 4; i++){
        read_next_bit(&discard_bit_ptr,msg);
    }

    for(uint32_t i = 0; i < (bmp_file->body).pixel_count; i++){
        if(!transform_bmp_file_pixel(transformation,msg, origin_fd, destination_fd,FIST_LOW_BIT_POSITION_LSB1)) return false;
    }

    return true;
}

/*---- BODY OPS ----*/



/***** PUBLIC FUNCTIONS *****/