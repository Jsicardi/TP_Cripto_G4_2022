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

    copy_bmp_file_metadata(&bmp_file, origin_fd, destination_fd);
    copy_bmp_file_offset(&bmp_file, origin_fd, destination_fd);

    char * msg = "";

    transform_bmp_file_body(&bmp_file, &no_transformation, msg, origin_fd, destination_fd);

    fclose(origin_fd);
    fclose(destination_fd);
    
    free(args);
}
