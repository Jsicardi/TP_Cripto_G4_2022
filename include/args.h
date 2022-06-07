#ifndef __ARGS_H_
#define __ARGS_H_

#define NONE 0
#define EMBED 1
#define EXTRACT 2

#define LSB1 1
#define LSB4 2
#define LSBI 3

#define AES_128 1
#define AES_192 2
#define AES_256 3
#define DES 4

#define ECB 1
#define CFB 2
#define OFB 3
#define CBC 4

struct stegobmp_args{
    int action;
    char * in_file;
    char * bmp_file;
    char * out_file;
    int steg;
    int enc;
    int mode;
    char * password;
};

void parse_args(const int argc, char ** argv, struct stegobmp_args * args);

#endif
