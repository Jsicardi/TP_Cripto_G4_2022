#include <stdio.h>
#include <stdlib.h>
#include "include/args.h"

struct stegobmp_args * args;

int main(int argc, char * argv[]){
    args = malloc(sizeof(struct stegobmp_args));
    printf("Hello\n");
    parse_args(argc,argv,args);
    printf("Action: %d In-file: %s BMP-file: %s out-file:%s Stego:%d Enc:%d Mode:%d Pass:%s\n", args->action,args->in_file,args->bmp_file,args->out_file,args->steg,args->enc,args->mode,args->password);
    free(args);
}
