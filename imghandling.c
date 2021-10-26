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

    if (w % 2 == 1) {
        original->width = w-1;
        assert(w - 1 > 0);
    }

    if (h % 2 == 1) {
        original->height = h-1;
        assert(h - 1 > 0);
    }
}

void read_header(FILE *fp, unsigned *width, unsigned *height)
{
    int read = fscanf(fp, "COMP40 Compressed image format 2\n%u %u",
                      width, height);
    assert(read == 2);
    int c = getc(fp);
    assert(c == '\n');
}

Pnm_ppm Pnm_ppm_new(unsigned width, unsigned height, unsigned denominator,
                   A2Methods_T methods)
{

    Pnm_ppm out = malloc(sizeof(*out));
    assert(out);

    out->width = width;
    out->height = height;
    out->denominator = denominator;
    out->methods = methods;
    Pnm_rgb new_pix = malloc(sizeof(*new_pix));
    assert(new_pix);
    out->pixels = methods->new(width, height, sizeof(*new_pix));
    free(new_pix);

    return out;
}
