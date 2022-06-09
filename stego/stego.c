#include "../include/stego/stego.h"

void get_lsbi_pattern(uint8_t byte, uint8_t *pattern){
    uint8_t sec_lsb;
    uint8_t third_lsb;
    *pattern = 0;

    get_bit_at(byte,6,&sec_lsb);
    get_bit_at(byte,5,&third_lsb);

    printf("2nd LSB: %d 3rd LSB: %d\n",sec_lsb,third_lsb);
    set_bit_at(pattern,7,third_lsb);
    set_bit_at(pattern,6,sec_lsb);

}

bool insert_lsb4_pixel(Pixel * pixel, BinaryMessage *msg){

    uint8_t next_bit;
    uint8_t* bytes[] = { &(pixel->blue), &(pixel->green), &(pixel->red) };

    for(int i = 0; i < PIXEL_SIZE; i++){

        for(int bit_pos = FIST_LOW_BIT_POSITION; bit_pos < BITS_IN_BYTE; bit_pos++){
            if(read_next_bit(&next_bit, msg)){
                set_bit_at(bytes[i], bit_pos, next_bit);
            }
        }

    }

    return true;
}


void get_inversion_functions(BmpBody body,BinaryMessage *msg, FILE * origin_fd,uint8_t (*pattern_functions[]) (uint8_t bit)){
    FILE * aux_origin_fd = origin_fd;

    BinaryMessage aux_msg;
    
    load_binary_message(msg->curr_byte_ptr, msg->last_byte_ptr, &aux_msg);

    //matrix has four rows as combinations of two bits (00 01 10 11) and 2 columns (modified pixels, non modified pixels)
    int modified_pixels_matrix[4][2] = {0}; 

    uint8_t read_byte;
    uint8_t lsb;
    uint8_t message_lsb;
    uint8_t pattern; 
    for(uint32_t i = 0; i < body.pixel_count * 3; i++){
        read_x_bytes(&read_byte,1,aux_origin_fd);
        get_lsbi_pattern(read_byte,&pattern);
        get_bit_at(read_byte,7,&lsb);
        if(!read_next_bit(&message_lsb,&aux_msg)){
            break;
        }
        printf("LSB: %d Pattern: %d Message LSB: %d\n",lsb,pattern,message_lsb);
        if(message_lsb == lsb){
            modified_pixels_matrix[pattern][1]++;
        }
        else{
            modified_pixels_matrix[pattern][0]++;
        }
    }

    for(uint32_t j = 0; j < 4; j++){
        printf("Pattern: %d, Modified bits: %d Conserved bits: %d\n",j,modified_pixels_matrix[j][0],modified_pixels_matrix[j][1]);
        if(modified_pixels_matrix[j][0] > modified_pixels_matrix[j][1]){
            pattern_functions[j] = &bit_inversion;
        }
        else{
            pattern_functions[j] = &bit_identity;
        }
    }



}