#include "stdint.h"
#include "stdio.h"

#define MAXPOOL_IN_SIZE 256
#define MAXPOOL_FEAT_SIZE 8
#define MAXPOOL_OUT_SIZE (MAXPOOL_IN_SIZE / 2)

volatile int8_t out1[MAXPOOL_OUT_SIZE][MAXPOOL_OUT_SIZE][MAXPOOL_FEAT_SIZE] = {};