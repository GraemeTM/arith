#include <stdlib.h>
#include <stdio.h>
#include "pnm.h"
#include "imghandling.h"
#include "a2plain.h"
#include "uarray2.h"

#define DENOM 255

void populate_random_colormap(int i, int j, A2Methods_UArray2 array2, 
                    A2Methods_Object *ptr, void *cl)
{
    Pnm_rgb new_pix = (Pnm_rgb)ptr;
    new_pix->red = rand() % DENOM;
    new_pix->green = rand() % DENOM;
    new_pix->blue = rand() % DENOM;
    (void)i;
    (void)j;
    (void)array2;
    (void)cl;
}

int main()
{
    A2Methods_T methods = uarray2_methods_plain;
    Pnm_ppm ppm = Pnm_ppm_new(500, 500, DENOM, methods);
    methods->map_default(ppm->pixels, populate_random_colormap, NULL);
    
    Pnm_ppmwrite(stdout, ppm);
}