#include <stdlib.h>
#include <string.h>

#include "../include/utils/byte_utils.h"
#include "../include/utils/binary_utils.h"
#include "../include/utils/file_utils.h"

#define DOT_CHAR '.'

/***** PRIVATE FUNCTIONS *****/

/*
    Given a file_descriptor, sets file_size to be the file_size
    corresponding to the file opened at the file_descriptor.
 */

bool get_file_size_from_file(uint32_t * file_size, FILE * file_descriptor);

/*
    Given a file_name, sets extension to be the last bytes before trailing \0
    following the last dot character.
 */

bool get_file_extension_from_filename(char * file_name, char ** extension, size_t * extension_size);

/*
    Given a file size, a file descriptor pointing to the start of the file contents, the extension and the extension size,
    copies:

        file_size || file_content || file_extension

    into msg_to_hide.
 */

bool cpy_msg_to_hide(uint8_t * msg_to_hide, uint32_t file_size, FILE * file_content, char * extension, size_t extension_size);

bool get_file_size_from_file(uint32_t * file_size, FILE * file_descriptor){

    // Goes to the end of the file
    if(fseek(file_descriptor, 0L, SEEK_END)) return false;

    // Gets offset which will be equal to the file size or -1L if something went wrong
    uint32_t offset = ftell(file_descriptor);
    if(offset == -1L) {
        return false;
    }
    *(file_size) = offset;

    // Get back to the beginning
    rewind(file_descriptor);

    return true;
}

bool get_file_extension_from_filename(char * file_name, char ** extension, size_t * extension_size){

    // Get last occurrence of the dot character. File extension should be found inmediatly after.
    char * dot_position = strrchr(file_name, DOT_CHAR);
    if(dot_position == NULL) return false;

    // Allocate memory for the extension length
    *(extension_size) = strlen(dot_position) + 1;
    *(extension)      = malloc(*(extension_size));
    if(*(extension) == NULL) return false;

    // Copy extension into allocated bytes
    strcpy(*(extension), dot_position);
    return true;
}

bool cpy_msg_to_hide(uint8_t * msg_to_hide, uint32_t file_size, FILE * file_content, char * extension, size_t extension_size){

    uint8_t size_arr[4];
    uint32_to_array_of_uint8(size_arr, file_size);

    // First copy file size
    memcpy(msg_to_hide, size_arr, 4);   

    // Then copy file content starting off after the file size
    if(!read_x_bytes(msg_to_hide + sizeof(file_size), file_size, file_content)) return false;

    // Finally copy the extension after the file content
    memcpy(msg_to_hide + sizeof(file_size) + file_size, extension, extension_size);

    return true;
}

/***** PRIVATE FUNCTIONS *****/




/***** PUBLIC FUNCTIONS *****/

bool load_from_file(BinaryMessage * msg, char * file_name){

    FILE * file_descriptor = fopen(file_name, READ_BYTES_MODE);
    if(file_descriptor == NULL) return false;

    uint32_t file_size;
    if(!get_file_size_from_file(&file_size, file_descriptor)) return false;

    char * extension;
    size_t extension_size;
    if(!get_file_extension_from_filename(file_name, &extension, &extension_size)) return false;
    
    uint8_t * msg_to_hide = malloc(sizeof(file_size) + file_size + extension_size);
    if(msg_to_hide == NULL) {
        free(extension);
        return false;
    }
    
    if(!cpy_msg_to_hide(msg_to_hide, file_size, file_descriptor, extension, extension_size)){
        free(extension);
        free(msg_to_hide);
        return false;
    } 

    free(extension);

    if(!load_binary_message(msg_to_hide, msg_to_hide + sizeof(file_size) + file_size + extension_size - 1, msg)){
        free(msg_to_hide);
        return false;
    }
    
    if(fclose(file_descriptor) == EOF) return false;

    return true;
}

bool close_loaded_file(BinaryMessage * msg){
    return unload_binary_message(msg, true);
}

bool load_to_file(BinaryMessage * msg, char * file_name, uint32_t file_size){
    printf("1\n");

    if(file_size < 0){
        if(!read_next_i32(&file_size, msg))         return false;
    }

    printf("2\n");

    BinaryMessage file_content;

    if(!copy_binary_message(msg, &file_content))    return false;

    printf("3\n");

    if(!skip_ahead_of_message(file_size, msg))      return false;

    printf("4\n");
    
    size_t extension_size = strlen(msg->curr_byte_ptr), file_name_size = strlen(file_name);

    

    char * full_file_name = malloc(file_name_size + extension_size + 1);
    if(full_file_name == NULL) return false;

    strncpy(full_file_name, file_name, file_name_size);
    strncpy(full_file_name + file_name_size, msg->curr_byte_ptr, extension_size);
    full_file_name[file_name_size + extension_size] = 0;
    
    FILE * file_descriptor = fopen(full_file_name, WRITE_BYTES_MODE);
    if(file_descriptor == NULL) {
        free(full_file_name);
        return false;
    }

    if(!write_x_bytes(file_content.curr_byte_ptr, file_size, file_descriptor)){
        free(full_file_name);
        fclose(file_descriptor);
        return false;
    }

    free(full_file_name);
    
    if(fclose(file_descriptor) == EOF) return false;

    return true;
}

/***** PUBLIC FUNCTIONS *****/