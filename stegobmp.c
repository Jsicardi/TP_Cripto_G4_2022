#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "include/utils/args.h"
#include "include/utils/byte_utils.h"
#include "include/bmp/bmp_file.h"

#define READ_BYTES_MODE "r"
#define WRITE_BYTES_MODE "w"

struct stegobmp_args * args;

bool no_transformation(Pixel * pixel, char * msg){ return true; }

int main(int argc, char * argv[]){
    args = malloc(sizeof(struct stegobmp_args));
    printf("Hello\n");
    parse_args(argc,argv,args);
    printf("Action: %d In-file: %s BMP-file: %s out-file:%s Stego:%d Enc:%d Mode:%d Pass:%s\n", args->action,args->in_file,args->bmp_file,args->out_file,args->steg,args->enc,args->mode,args->password);
    
    FILE * origin_fd      = fopen(args->bmp_file, READ_BYTES_MODE);
    FILE * destination_fd = fopen(args->out_file, WRITE_BYTES_MODE);

    BmpFile bmp_file;

    if(!copy_bmp_file_metadata(&bmp_file, &bmp_file_is_uncompressed, origin_fd, destination_fd)) {
        
        if(bmp_file_is_compressed(&bmp_file)){
            return 1; // File is compressed
        }

        return 2; // Error parsing metadata from origin_fd
    }

    if(!copy_bmp_file_offset(&bmp_file, origin_fd, destination_fd)) return 3; // Could not copy BMP File's offset bytes. Could be that metadata is not correct.

    char * msg = "";

    if(!transform_bmp_file_body(&bmp_file, &no_transformation, msg, origin_fd, destination_fd)) return 4; // Error copying body pixels

    // Reset bmp_file variable for security measures
    clean_bmp_file_structure(&bmp_file);

    fclose(origin_fd);
    fclose(destination_fd);

    // Reset file_descriptors' variables for security measures
    origin_fd      = NULL;
    destination_fd = NULL;
    

    free(args);
}
