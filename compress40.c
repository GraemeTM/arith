#include <stdio.h>
#include <stdlib.h>
#include "compress40.h"
#include "imghandling.h"
#include "bitpack.h"
#include "arith40.h"
#include "pnm.h"
#include "assert.h"
#include "a2blocked.h"
#include "color_math.h"


void compress40(FILE *input)
{
    A2Methods_T methods = uarray2_methods_blocked;
    
    Pnm_ppm ppm = get_ppm(input, methods);
    
    printf("%u, %u\n", ppm->width, ppm->height);

    trim_edges(ppm);

    float d = (float)ppm->denominator;
    A2Methods_UArray2 pixmap = ppm->pixels;
    for(int r = 0; r < (int)ppm->height; r += 2)
    {
        for(int c = 0; c < (int)ppm->width; c += 2)
        {
            rgb_T tl = rgb_from_pnm((Pnm_rgb)methods->at(pixmap, c, r), d);
            rgb_T tr = rgb_from_pnm((Pnm_rgb)methods->at(pixmap, c + 1, r), d);
            rgb_T bl = rgb_from_pnm((Pnm_rgb)methods->at(pixmap, c, r + 1), d);
            rgb_T br = rgb_from_pnm((Pnm_rgb)methods->at(pixmap, c+1, r+1), d);
            
            
        }
    }
}

void decompress40(FILE *input)
{
    (void)input;
}