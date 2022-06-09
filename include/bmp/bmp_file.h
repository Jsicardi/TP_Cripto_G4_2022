#ifndef __BMP_FILE_H_
#define __BMP_FILE_H_

#include <stdint.h>
#include <stdbool.h>

#include "../utils/binary_utils.h"
#include "bmp_header.h"
#include "bmp_info.h"

#define PIXEL_SIZE 3

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
    uint32_t pixel_count;
} BmpBody;

typedef struct {
    BmpHeader header;
    BmpInfo   info;
    BmpBody   body;
} BmpFile;

/*
    Sets all BmpFile fields to 0 to prevent leaking information into memory
    and becoming a potential security vulnerability.
 */

void clean_bmp_file_structure(BmpFile * bmp_file);

/*---- META DATA OPS ----*/

/*
    Given an empty BmpFile structure and an unread file_descriptor pointing to a BMP File,
    parses its metadata into the BmpFile structure.

    It sets the file_descriptor to be pointing after the BMP File head and info.
    Parsing posible offset bits is needed as BMP File body may not be found
    inmediatly afterwards.

    After reading the metadata and before getting the BMP File body properties,
    a is_valid_metadata operation is performed before proceeding
 */

bool read_bmp_file_metadata(BmpFile * bmp_file, bool (*is_valid_metadata)(BmpFile*), FILE * file_descriptor);

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

    After reading the metadata and before getting the BMP File body properties,
    a is_valid_metadata operation is performed before proceeding.
    
    If the is_valid_metadata predicate determines that the metadata obtained by the origin_fd is not
    up to par, the write operation is not performed.
 */

bool copy_bmp_file_metadata(BmpFile * bmp_file, bool (*is_valid_metadata)(BmpFile*), FILE * origin_fd, FILE * destination_fd);

/*
    Returns whether the compression bits are != 0 in a bmp_file structure.
 */

bool bmp_file_is_compressed(BmpFile * bmp_file);

/*
    Returns whether the compression bits are == 0 in a bmp_file structure.
 */

bool bmp_file_is_uncompressed(BmpFile * bmp_file);

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

bool transform_bmp_file_pixel(bool (*transformation) (Pixel*, BinaryMessage *), BinaryMessage * msg, FILE * origin_fd, FILE * destination_fd);

/*---- PIXEL OPS ----*/


/*---- BODY OPS ----*/

/*
    Given a BmpFile structure where metadata was parsed, a transformation, an origin file_descriptor and a destination file_descriptor
    both pointing at the beginning of the corresponding BMP Files bodies, reads pixel by pixel from the origin file_descriptor,
    applies a transformation for each and everyone and writes the transformed pixels onto the destination file_descriptor's body
 */

bool transform_bmp_file_body(BmpFile * bmp_file, bool (*transformation) (Pixel*, BinaryMessage*), BinaryMessage * msg, FILE * origin_fd, FILE * destination_fd);

/*
    Given a BmpFile structure where metadata was parsed, an origin file_descriptor and a destination file_descriptor
    both pointing at the beginning of the corresponding BMP Files bodies, applies the LSBI stego algorithm and writes the transformed pixels onto the destination file_descriptor's body
 */

bool transform_bmp_file_body_lsbi(BmpFile *bmp_file, BinaryMessage * msg, FILE * origin_fd, FILE * destination_fd);

/*---- BODY OPS ----*/



#endif