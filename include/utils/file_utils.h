#ifndef __FILE_UTILS_H_
#define __FILE_UTILS_H_

#include "binary_utils.h"

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#define READ_BYTES_MODE "r"
#define WRITE_BYTES_MODE "w"

/*
    Given a file_name, loads the file contents onto a BinaryMessage structure.

    REMEMBER: The BinaryMessage is saved in heap so one MUST call close_file
    when BinaryMessage is no longer used.
 */

bool load_from_file(BinaryMessage * msg, char * file_name);

/*
    Cleans BinaryMessage structure and frees the message.
*/

bool close_loaded_file(BinaryMessage * msg);

bool load_to_file(BinaryMessage * msg, char * file_name, uint32_t file_size);

#endif