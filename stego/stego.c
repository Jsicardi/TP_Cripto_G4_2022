#include "../include/stego/stego.h"

bool get_lsbi_pattern(uint8_t byte, uint8_t *pattern);

bool message_can_be_stego(BmpFile * bmp, int lsb_type, BinaryMessage * msg){

    size_t message_size_in_bytes, available_hiding_place;
    uint32_t bmp_body_size_in_pixels;

    if(!get_binary_message_size(&message_size_in_bytes, msg)) return false;
    get_bmp_file_body_size(&bmp_body_size_in_pixels, bmp);

    switch (lsb_type) {
        case LSB1:
            /*
                Multiply body size in pixels by 3 to get body size in bytes. Afterward divide by 8 as we need 8 bytes to
                store 1 byte of message (a bytes hides 1 bit).
             */

            available_hiding_place = bmp_body_size_in_pixels*3 / 8;

            if(message_size_in_bytes < 0 || available_hiding_place <= 0 || message_size_in_bytes > available_hiding_place) return false;
            return true;
    
        case LSB4:
            /*
                Multiply body size in pixels by 3 to get body size in bytes. Afterward divide by 2 as we need 2 bytes to
                store 1 byte of message (a bytes hides 4 bits).
             */
            
            available_hiding_place = bmp_body_size_in_pixels*3 / 2;

            if(message_size_in_bytes < 0 || available_hiding_place <= 0 || message_size_in_bytes > available_hiding_place) return false;
            return true;

        case LSBI:
            /*
                Multiply body size in pixels by 3 to get body size in bytes. Afterward divide by 8 as we need 8 bytes to
                store 1 byte of message (a bytes hides 1 bit). On if, add 4 aditional bytes in comp (needed to store the
                4 bits for pattern inversion)
             */
            available_hiding_place = bmp_body_size_in_pixels*3 / 8;
            if(message_size_in_bytes < 0 || available_hiding_place <= 0 || (message_size_in_bytes + 4) > available_hiding_place) return false;
            return true;
    }
    return false;
}

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

bool get_lsb_pixel(Pixel * pixel, BinaryMessage *writeable_msg, int first_low_bit_position){

    uint8_t next_bit;
    uint8_t* bytes[] = { &(pixel->blue), &(pixel->green), &(pixel->red) };

    for(int i = 0; i < PIXEL_SIZE; i++){

        for(int bit_pos = first_low_bit_position; bit_pos < BITS_IN_BYTE; bit_pos++){
            if(read_next_bit(&next_bit, writeable_msg)){
                set_bit_at(bytes[i], bit_pos, next_bit);
            }
        }

    }

    return true;
}

bool get_lsbi_pattern(uint8_t byte, uint8_t *pattern){
    uint8_t sec_lsb;
    uint8_t third_lsb;
    *pattern = 0;

    get_bit_at(byte,6,&sec_lsb);
    get_bit_at(byte,5,&third_lsb);

    //printf("2nd LSB: %d 3rd LSB: %d\n",sec_lsb,third_lsb);
    set_bit_at(pattern,7,sec_lsb);
    set_bit_at(pattern,6,third_lsb);
    return true;
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

/*** SNATCHER FUNCTIONS ***/

/*
    Auxiliary function for lsb_snatcher. Given a writeable_binary_message, writes the bit in the byte at
    bit_position position onto the BinaryMessage.
 */

bool snatched_bit_into_binary_message(uint8_t * byte, uint8_t bit_position, BinaryMessage * msg,uint8_t (*pattern_functions[]) (uint8_t bit));

/*
    Given a byte pointer, a LsbSnatcherCtx and a BinaryMessage, loads the cypher onto the BinaryMessage
    and sets the LsbSnatcherCtx acordingly.

    first_low_bit_position will determine the starting bit from which to retrive
    1 -> last bit
    4 -> last four (4) bits
    and so.
*/

bool enc_lsb_snatcher(uint8_t * byte, void * ctx, BinaryMessage * msg, uint8_t first_low_bit_position,bool is_lsbi);

bool new_lsb_snatcher_ctx(LsbSnatcherCtx * snatcher_ctx){
    
    snatcher_ctx->state     = INIT_SNATCH;
    snatcher_ctx->enc_bytes = 0;
    for(int i = 0; i < 4; i++){
        snatcher_ctx->pattern_functions[i] = &bit_identity;
    }

    snatcher_ctx = NULL;
    return true;
}

bool snatched_bit_into_binary_message(uint8_t * byte, uint8_t bit_position, BinaryMessage * msg,uint8_t (*pattern_functions[]) (uint8_t bit)){
    uint8_t bit;

    if(!get_bit_at(*byte, bit_position, &bit)){
        if(!unload_binary_message(msg, true)) return false;
        return false;
    }

    /*Getting the pattern is used for LSBI. If this function is used with LSB1 and LSB4, pattern_functions are always the identity*/
    uint8_t pattern;
    if(!get_lsbi_pattern(*byte,&pattern)){
        if(!unload_binary_message(msg, true)) return false;
        return false;
    }

    if(!write_next_bit(pattern_functions[pattern](bit), msg)){
        if(!unload_binary_message(msg, true)) return false;
        return false;
    }

    return true;
}

bool enc_lsb_snatcher(uint8_t * byte, void * ctx, BinaryMessage * msg, uint8_t first_low_bit_position, bool is_lsbi){
    
    LsbSnatcherCtx * snatcher_ctx = (LsbSnatcherCtx *) ctx;

    switch(snatcher_ctx->state){
        case INIT_SNATCH:
        {
            size_t bytes_to_reserve = sizeof(uint32_t);
            if(is_lsbi){
                bytes_to_reserve = 1;
            } 
            
            if(!writeable_binary_message(bytes_to_reserve, &(snatcher_ctx->w_msg))) {
                if(!unload_binary_message(&(snatcher_ctx->w_msg), true)) return false;
                return false;
            }
            if(!copy_binary_message(&(snatcher_ctx->w_msg), msg)){
                    if(!unload_binary_message(&(snatcher_ctx->w_msg), true)) return false;
                    return false;
            }     

            if(is_lsbi){
                //skip first 4 bits to write latter the pattern ones
                for(int i = 0; i < 4; i++){
                    write_next_bit(0,&(snatcher_ctx->w_msg));
                }
                snatcher_ctx->state = PATTERN_SNATCH;
            }
            else{
                snatcher_ctx->state = SIZE_SNATCH;
            }
            //Recursive call to start on PATTERN_SNATCH or SIZE_PATTERN and process the first bit
            return enc_lsb_snatcher(byte,ctx,msg,first_low_bit_position,is_lsbi);
        }
        case PATTERN_SNATCH:
            {
                for(uint8_t i = first_low_bit_position; i < BITS_IN_BYTE; i++){

                    if(!snatched_bit_into_binary_message(byte, i, &(snatcher_ctx->w_msg),snatcher_ctx->pattern_functions)) return false;
                    
                    /*if 4 pattern bits were already read*/
                    if(snatcher_ctx->w_msg.curr_byte_ptr > snatcher_ctx->w_msg.last_byte_ptr){

                        uint8_t bit = 0;

                        //skip filler bits added
                        for(int curr_bit=0;curr_bit < 4;curr_bit++){
                            if(!read_next_bit(&bit, msg)){
                                if(!unload_binary_message(&(snatcher_ctx->w_msg), true))            return false;
                                return false;
                            }

                        }

                        /*Read inversion bits, if bit is 1, change the pattern function from identity to inversion*/
                        for(int curr_bit = 0; curr_bit < 4; curr_bit++) {

                            if(!read_next_bit(&bit, msg)){
                                if(!unload_binary_message(&(snatcher_ctx->w_msg), true))            return false;
                                return false;
                            }

                            if(bit == 1){
                                snatcher_ctx->pattern_functions[curr_bit] = &bit_inversion;
                            }
                        }
                        
                        // Unloads current binary message (now empty). And loads a new one to store the size bytes.
                        if(!unload_binary_message(&(snatcher_ctx->w_msg), true))                        return false;
                        if(!writeable_binary_message(sizeof(uint32_t), &(snatcher_ctx->w_msg)))  return false;
                        if(!copy_binary_message(&(snatcher_ctx->w_msg), msg)){
                            if(!unload_binary_message(&(snatcher_ctx->w_msg), true)) return false;
                            return false;
                        }
                        snatcher_ctx->state = SIZE_SNATCH;
                    }   
                }
            }    
            return true;
        case SIZE_SNATCH:
            {
                for(uint8_t i = first_low_bit_position; i < BITS_IN_BYTE; i++){

                    if(!snatched_bit_into_binary_message(byte, i, &(snatcher_ctx->w_msg),snatcher_ctx->pattern_functions)) return false;

                    if(snatcher_ctx->w_msg.curr_byte_ptr > snatcher_ctx->w_msg.last_byte_ptr){

                        /* 
                            Loads bits stored in binary message into the variable enc_bytes.
                            enc_bytes will contain the length of the cypher after execution
                         */

                        snatcher_ctx->enc_bytes = 0;

                        uint8_t bit;

                        for(int curr_byte = 0; curr_byte < sizeof(uint32_t); curr_byte++) {

                            for(int curr_bit = 0; curr_bit < BITS_IN_BYTE; curr_bit++) {

                                if(!read_next_bit(&bit, msg)){
                                    if(!unload_binary_message(&(snatcher_ctx->w_msg), true))            return false;
                                    return false;
                                }
                                snatcher_ctx->enc_bytes <<= 1;
                                snatcher_ctx->enc_bytes = snatcher_ctx->enc_bytes | bit;
                            }
                        }

                        // Unloads current binary message (now empty). And loads a new one to store the cyphered text.
                        if(!unload_binary_message(&(snatcher_ctx->w_msg), true))                        return false;
                        if(!writeable_binary_message(snatcher_ctx->enc_bytes, &(snatcher_ctx->w_msg)))  return false;
                        if(!copy_binary_message(&(snatcher_ctx->w_msg), msg)){
                            if(!unload_binary_message(&(snatcher_ctx->w_msg), true)) return false;
                            return false;
                        }
                        snatcher_ctx->state = MESSAGE_SNATCH;
                    }
                }
            }
            return true;
        case MESSAGE_SNATCH:
            {
                for(uint8_t i = first_low_bit_position; i < BITS_IN_BYTE; i++){

                    if(!snatched_bit_into_binary_message(byte, i, &(snatcher_ctx->w_msg),snatcher_ctx->pattern_functions)) return false;

                    if(snatcher_ctx->w_msg.curr_byte_ptr > snatcher_ctx->w_msg.last_byte_ptr){
                        snatcher_ctx->state = FINISHED_SNATCH;
                    }
                }
            }
            return true;
        case FINISHED_SNATCH:
            return true;
    }
    return false;
}

bool enc_lsb1_snatcher(uint8_t * byte, void * ctx, BinaryMessage * msg){
    return enc_lsb_snatcher(byte, ctx, msg, FIST_LOW_BIT_POSITION_LSB1, false);
}

bool enc_lsb4_snatcher(uint8_t * byte, void * ctx, BinaryMessage * msg){
    return enc_lsb_snatcher(byte, ctx, msg, FIST_LOW_BIT_POSITION_LSB4, false);
}

bool enc_lsbi_snatcher(uint8_t * byte, void *ctx, BinaryMessage *msg){
    return enc_lsb_snatcher(byte, ctx, msg, FIST_LOW_BIT_POSITION_LSB1, true);
}

bool no_enc_lsb_snatcher(uint8_t * byte, void * ctx, BinaryMessage * msg, uint8_t first_low_bit_position, bool is_lsbi) {
    LsbSnatcherCtx * snatcher_ctx   = (LsbSnatcherCtx *) ctx;

    if(snatcher_ctx->state == EXTENSION_SNATCH){
        for(uint8_t i = first_low_bit_position; i < BITS_IN_BYTE; i++){
            if(!snatched_bit_into_binary_message(byte, i, &(snatcher_ctx->w_msg), snatcher_ctx->pattern_functions)) return false;

            if(snatcher_ctx->w_msg.curr_byte_ptr > snatcher_ctx->w_msg.last_byte_ptr){
                if(*(snatcher_ctx->w_msg.last_byte_ptr) == 0){
                    snatcher_ctx->state = FINISHED_SNATCH;
                } else {
                    if(!add_writeable_bytes(1, &(snatcher_ctx->w_msg))){
                        if(!unload_binary_message(&(snatcher_ctx->w_msg), true)) return false;
                        return false;
                    }
                    if(!copy_binary_message(&(snatcher_ctx->w_msg), msg)){
                        if(!unload_binary_message(&(snatcher_ctx->w_msg), true)) return false;
                        return false;
                    }
                    msg->curr_byte_ptr  = msg->message;
                    msg->curr_bit       = 0;
                }
                
            }
        }
        return true;
    }
    lsb_snatcher_state_t prev_state = snatcher_ctx->state;

    bool ret_val = enc_lsb_snatcher(byte, ctx, msg, first_low_bit_position, is_lsbi);
    if(ret_val == false) return false;

    // Captures state after finishing reading message and sets it so it reads the extension
    if(prev_state == MESSAGE_SNATCH && snatcher_ctx->state == FINISHED_SNATCH){
        snatcher_ctx->state = EXTENSION_SNATCH;
        if(!add_writeable_bytes(1, &(snatcher_ctx->w_msg))){
            if(!unload_binary_message(&(snatcher_ctx->w_msg), true)) return false;
            return false;
        }
        if(!copy_binary_message(&(snatcher_ctx->w_msg), msg)){
            if(!unload_binary_message(&(snatcher_ctx->w_msg), true)) return false;
            return false;
        }
        msg->curr_byte_ptr  = msg->message;
        msg->curr_bit       = 0;
    }
    return true;
}

bool no_enc_lsb1_snatcher(uint8_t * byte, void * ctx, BinaryMessage * msg){
    return no_enc_lsb_snatcher(byte, ctx, msg, FIST_LOW_BIT_POSITION_LSB1, false);
}

bool no_enc_lsb4_snatcher(uint8_t * byte, void * ctx, BinaryMessage * msg){
    return no_enc_lsb_snatcher(byte, ctx, msg, FIST_LOW_BIT_POSITION_LSB4, false);
}

bool no_enc_lsbi_snatcher(uint8_t * byte, void *ctx, BinaryMessage *msg){
    return no_enc_lsb_snatcher(byte, ctx, msg, FIST_LOW_BIT_POSITION_LSB1, true);
}

/*** SNATCHER FUNCTIONS ***/