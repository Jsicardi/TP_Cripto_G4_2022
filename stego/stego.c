#include "../include/stego/stego.h"

void get_lsbi_pattern(uint8_t byte, uint8_t *pattern);

bool insert_lsb_pixel(Pixel * pixel, BinaryMessage *msg, int first_low_bit_position){

    uint8_t next_bit;
    uint8_t* bytes[] = { &(pixel->blue), &(pixel->green), &(pixel->red) };

    for(int i = 0; i < PIXEL_SIZE; i++){

        for(int bit_pos = first_low_bit_position; bit_pos < BITS_IN_BYTE; bit_pos++){
            if(read_next_bit(&next_bit, msg)){
                set_bit_at(bytes[i], bit_pos, next_bit);
            }
        }

    }

    return true;
}

void get_lsbi_pattern(uint8_t byte, uint8_t *pattern){
    uint8_t sec_lsb;
    uint8_t third_lsb;
    *pattern = 0;

    get_bit_at(byte,6,&sec_lsb);
    get_bit_at(byte,5,&third_lsb);

    //printf("2nd LSB: %d 3rd LSB: %d\n",sec_lsb,third_lsb);
    set_bit_at(pattern,7,third_lsb);
    set_bit_at(pattern,6,sec_lsb);

}

void get_inversion_functions(BmpBody body,BinaryMessage *msg, FILE * origin_fd,uint8_t (*pattern_functions[]) (uint8_t bit), uint8_t * inversion_byte){
    int current_pos = ftell(origin_fd);
    
    //skip the 4 bytes used for inversion bits
    fseek(origin_fd,4,SEEK_CUR);

    BinaryMessage aux_msg;
    
    load_binary_message(msg->curr_byte_ptr, msg->last_byte_ptr, &aux_msg);

    //matrix has four rows as combinations of two bits (00 01 10 11) and 2 columns (modified pixels, non modified pixels)
    int modified_pixels_matrix[4][2] = {0}; 

    uint8_t read_byte;
    uint8_t lsb;
    uint8_t message_bit;
    uint8_t pattern; 
    for(uint32_t i = 0; i < body.pixel_count * 3; i++){
        read_x_bytes(&read_byte,1,origin_fd);
        get_lsbi_pattern(read_byte,&pattern);
        get_bit_at(read_byte,7,&lsb);
        if(!read_next_bit(&message_bit,&aux_msg)){
            break;
        }
        //printf("LSB: %d Pattern: %d Message LSB: %d\n",lsb,pattern,message_lsb);
        if(message_bit == lsb){
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
            set_bit_at(inversion_byte,4+j,1);
        }
        else{
            pattern_functions[j] = &bit_identity;
            set_bit_at(inversion_byte,4+j,0);
        }
    }

    //rewind file to original position before inversion analysis
    rewind(origin_fd);
    fseek(origin_fd,current_pos,SEEK_CUR);

}

bool invert_lsbi_message_bits(BmpBody body,BinaryMessage *msg, FILE * origin_fd,uint8_t (*pattern_functions[]) (uint8_t bit)){
    int current_pos = ftell(origin_fd);

    //skip the 4 bytes used for inversion bits
    fseek(origin_fd,4,SEEK_CUR);

    BinaryMessage aux_msg;
    
    load_binary_message(msg->curr_byte_ptr, msg->last_byte_ptr, &aux_msg);

    uint8_t read_byte;
    uint8_t message_bit;
    uint8_t pattern;
    uint8_t bit_position = 0; 
    uint8_t msg_index = 0;
    for(uint32_t i = 0; i < body.pixel_count * 3; i++){
        read_x_bytes(&read_byte,1,origin_fd);
        get_lsbi_pattern(read_byte,&pattern);
        if(!read_next_bit(&message_bit,&aux_msg)){
            break;
        };
        set_bit_at(msg->curr_byte_ptr + msg_index, bit_position,pattern_functions[pattern](message_bit));
        bit_position++;
        if(bit_position == BITS_IN_BYTE){
            bit_position = 0;
            msg_index++;
        } 
    }

    //rewind file to original position before current analysis
    rewind(origin_fd);
    fseek(origin_fd,current_pos,SEEK_CUR);
    return true;
}
