#ifndef __PACKING_UTILS_H_
#define __PACKING_UTILS_H_

#include <stdio.h>
#include <stdbool.h>
#include "binary_utils.h"
#include "args.h"

bool pack_message_from_file(struct stegobmp_args * args, BinaryMessage * msg);

#endif