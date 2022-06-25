#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include "../include/utils/args.h"

void show_help(){
    fprintf(stdout,
        "Uso stegobmp\nParametros:\n\n"
        "    -h          Imprime la ayuda y termina.\n\n"
        "    --embed       Indica que se va a ocultar informacion. Requiere parametros --in --out --steg y -p\n\n"
        "    --in <message>          Archivo a ocultarse\n\n"
        "    -p <carrier.bmp>          Archivo bmp portador\n\n"
        "    --out <out_file>         Archivo de salida (bmp portador con mensaje incrustado)\n\n"
        "    --steg <LSB1|LSB4|LSBI>         Metodo de esteganografía a usar \n\n"
        "    -a <aes128|aes192|aes256|des>         Metodo de encriptacion a utilizar \n\n"
        "    -m <ecb|cfb|ofb|cbc>         Metodo de encriptacion a utilizar \n\n"
        "    --pass <password>         Metodo de encriptacion a utilizar \n\n"

   );
}

void parse_args(const int argc, char ** argv, struct stegobmp_args * args){

    memset(args,0,sizeof(*args));
    args->enc = NONE;
    args->mode = NONE;
    args->password = NONE;
    args->action = NONE;

    struct option longopts[] = {
            {"embed",no_argument,0,0},
            {"extract",no_argument,0,1},
            {"in",required_argument,0,2},
            {"out",required_argument,0,3},
            {"steg",required_argument,0,4},
            {"pass",required_argument,0,5},
    };

    int char_ret = 0, index = 0;
    while(char_ret != -1) {
        char_ret = getopt_long(argc, argv, "hp:a:m:",longopts,&index);
        switch (char_ret) {
            case 'h':
                show_help();
                exit(0);
            case 'p':
                args->bmp_file = optarg;
                break;
            case 'a':
                if(strcmp(optarg, "aes128") == 0){
                    args->enc = AES_128;
                }
                else if(strcmp(optarg, "aes192") == 0){
                    args->enc = AES_192;
                }
                else if(strcmp(optarg, "aes256") == 0){
                    args->enc = AES_256;
                }
                else if(strcmp(optarg, "des") == 0){
                    args->enc = DES;
                }
                else{
                    printf("Unknown encryption algortihm %s\n", optarg);
                    exit(1);
                }
                break;
            case 'm':
                if(strcmp(optarg, "ecb") == 0){
                    args->mode = ECB;
                }
                else if(strcmp(optarg, "cfb") == 0){
                    args->mode = CFB;
                }
                else if(strcmp(optarg, "ofb") == 0){
                    args->mode = OFB;
                }
                else if(strcmp(optarg, "cbc") == 0){
                    args->mode = CBC;
                }
                else{
                    printf("Unknown encryption mode %s\n", optarg);
                    exit(1);
                }
                break;
            case 0:
                args->action = EMBEED;
                break;
            case 1:
                args->action = EXTRACT;
                break;
            case 2:
                args->in_file = optarg;
                break;
            case 3:
                args->out_file = optarg;
                break;
            case 4:
                if(strcmp(optarg, "LSB1") == 0){
                    args->steg = LSB1;
                }
                else if(strcmp(optarg, "LSB4") == 0){
                    args->steg = LSB4;
                }
                else if(strcmp(optarg, "LSBI") == 0){
                    args->steg = LSBI;
                }
                else{
                    printf("Unknown steg algorithm %s\n", optarg);
                    exit(1);
                }
                break;
            case 5:
                args->password = optarg;
                break;
            case -1:
                break;    
            default:
                printf("Unknown argument %c\n", char_ret);
                exit(1);
                break;
        }
    }
    if(args->action == NONE){
        printf("Action parameter required\n");
        exit(1);
    }
    if(args->action == EMBEED && (args->in_file == 0 || args->bmp_file == 0 || args->out_file == 0 || args->steg == NONE)){
        printf("--in,--out,-p and --steg parameters are required for action embed\n");
        exit(1);
    }
    if(args->action == EXTRACT && (args->bmp_file == 0 || args->out_file == 0 || args->steg == NONE)){
        printf("--out,-p and --steg parameters are required for action extract\n");
        exit(1);
    }
    if(args->password != 0 && args->enc == NONE){
        args->enc = AES_128;
    }
    if(args->password != 0 && args->mode == NONE){
        args->enc = CBC;
    }
    if((args->enc != NONE || args->mode != NONE) && args->password == 0){
        args->enc = NONE;
        args->mode = NONE;
    }


}
