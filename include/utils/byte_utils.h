#ifndef __BYTE_UTILS_H_
#define __BYTE_UTILS_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/*
    Reads bytes_to_read bytes from a file_descriptor (a binary file) onto
    the memory positions referenced by buffer

    returns false on failure to read from the file_descriptor
 */

bool read_x_bytes(void* buffer, size_t bytes_to_read, FILE* file_descriptor);

/*
    Writes bytes_to_read bytes to a file_descriptor (a binary file) from
    the memory positions referenced by buffer

    returns false on failure to write to the file_descriptor
 */

bool write_x_bytes(void* buffer, size_t bytes_to_write, FILE* file_descriptor);

/*
    Reads bytes_to_copy from an origin file descriptor (a binary file) and writes
    each and every byte to a destination_fd

    returns false on failure to read from the origin_fd or write to the destination_fd.
*/

bool copy_x_bytes(size_t bytes_to_copy, FILE* origin_fd, FILE* destination_fd);

#endif