#include <stdlib.h>
#include <stdio.h>
#include "pnm.h"
#include "assert.h"
#include "a2methods.h"


Pnm_ppm get_ppm(FILE *fp, A2Methods_T methods)
{
    Pnm_ppm ppm = Pnm_ppmread(fp, methods);
    assert(ppm);

    return ppm;
}

void trim_edges(Pnm_ppm original)
{
    int w = original->width, h = original->height;

    if(w % 2 == 1)
    {
        original->width = w-1;
        assert(w - 1 > 0);
    }
    
    if(h % 2 == 1)
    {
        original->height = h-1;
        assert(h - 1 > 0);
    } 
}