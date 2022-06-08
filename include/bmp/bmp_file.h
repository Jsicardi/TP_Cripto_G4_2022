#ifndef __BMP_FILE_H_
#define __BMP_FILE_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "bmp_header.h"
#include "bmp_info.h"

#define PIXEL_SIZE 24

/* 
    A Pixel is comprised of a single byte for:
        - Blue
        - Green
        - Red
    
    Totals 3 bytes or 24 bits.
    Color range is from 0 to 255.
 */

typedef struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
} Pixel;

/*
    BMP Files have 54 bytes before its body. Those bytes are divided into:
        - 14 BmpHeader bytes
        - 40 BmpInfo bytes
    
    Checking whether after those 54 bytes one can find the BMP File body or there
    are offset bytes is advised.

    Alternatively, calling read_bmp_file_metadata ensures bytes read afterwards belong to the body.

    read_bmp_file_metadata should be called upon just once per reading instance.
 */

typedef struct {
    BmpHeader header;
    BmpInfo   info;
} BmpFile;



/*---- META DATA OPS ----*/

/*
    Given an empty BmpFile structure and an unread file_descriptor pointing to a BMP File,
    parses its metadata into the BmpFile structure.

    It sets the file_descriptor to be pointing after the BMP File head and info.
    Parsing posible offset bits is needed as BMP File body may not be found
    inmediatly afterwards.
 */

bool read_bmp_file_metadata(BmpFile * bmp_file, FILE * file_descriptor);

/*
    Given a filled out BmpFile structure and a file_descriptor pointing to the beginning of a BMP File,
    writes the BmpFile structure metadata onto the file_descriptor.

    It sets the file_descriptor to be pointing after the BMP File head and info.
    Parsing posible offset bits is needed as BMP File body may not be found
    inmediatly afterwards.
 */

bool write_bmp_file_metadata(BmpFile * bmp_file, FILE * file_descriptor);

/*
    Reads the BMP File metadata from origin_fd and writes it to the destination_fd.
    Afterwards the BmpFile structure is filled out with the information now in both file_descriptors.

    It is of outmost important that both file_descriptors (origin and destination) are pointing at
    the beginning of the BMP File before calling this operation.

    Afterwards, the file_descriptor is set to be pointing after the BMP File head and info.
    Parsing posible offset bits is needed as BMP File body may not be found
    inmediatly afterwards.
 */

bool copy_bmp_file_metadata(BmpFile * bmp_file, FILE * origin_fd, FILE * destination_fd);

/*---- META DATA OPS ----*/



/*---- OFFSET OPS ----*/

/*
    Given an origin and a destination file_descriptors pointing to the end of the BMP file metadata
    (after the Header and Info), it copies the offset bytes in between metadata and body.

    This operation should only be executed once per file after parsing the bmp_file_metadata.
 */

bool copy_bmp_file_offset(BmpFile * bmp_file, FILE * origin_fd, FILE * destination_fd);

/*---- OFFSET OPS ----*/



/*---- PIXEL OPS ----*/

/*
    Given a file_descriptor belonging to a BMP File pointing to a pixel at the body bytes,
    reads a single pixel and sets it on the pixel memory position.
 */

bool read_bmp_file_pixel(Pixel * pixel, FILE * file_descriptor);

/*
    Given a file_descriptor belonging to a BMP File pointing at the body bytes,
    writes a single pixel.
 */

bool write_bmp_file_pixel(Pixel * pixel, FILE * file_descriptor);

/*
    Given an origin file descriptor, a destination file descriptor, a transformation function and an optional msg,
    it transforms the current pixel at origin_fd and writes it onto the destination_fd.
*/

bool transform_bmp_file_pixel(bool (*transformation) (Pixel*, char *), char * msg, FILE * origin_fd, FILE * destination_fd);

/*---- PIXEL OPS ----*/

#endif