#ifndef __BYTE_UTILS_H_
#define __BYTE_UTILS_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/*
    Reads bytes_to_read bytes from a file_descriptor (a binary file) onto
    the memory positions referenced by buffer

    returns false on failure to read the file_descriptor
 */

bool read_x_bytes(void* buffer, size_t bytes_to_read, FILE* file_descriptor);

/*
    Reads bytes_to_drop from a file descriptor (a binary file) and discards
    each and every byte

    returns false on failure to read the file_descriptor
*/

bool drop_x_bytes(size_t bytes_to_drop, FILE* file_descriptor);

#endif