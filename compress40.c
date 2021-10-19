#include <stdio.h>
#include <stdlib.h>
#include "compress40.h"
#include "imghandling.h"
#include "bitpack.h"
#include "arith40.h"
#include "pnm.h"
#include "assert.h"
#include "a2blocked.h"


void compress40(FILE *input)
{
    A2Methods_T methods = uarray2_methods_blocked;
    
    Pnm_ppm ppm = get_ppm(input, methods);
    
    printf("%u, %u\n", ppm->width, ppm->height);

    trim_edges(ppm);

    printf("%u, %u\n", ppm->width, ppm->height);
}

void decompress40(FILE *input)
{
    (void)input;
}