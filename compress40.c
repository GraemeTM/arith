#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "compress40.h"
#include "imghandling.h"
#include "bitpack.h"
#include "arith40.h"
#include "pnm.h"
#include "assert.h"
#include "a2blocked.h"
#include "color_math.h"
#include <math.h>

uint32_t get_word_from_chunk(int col, int row, A2Methods_T methods,
                             Pnm_ppm ppm);
DCTSpace_T scale_dct_values(DCTSpace_T dct);
float bound_to_range(float val, float min, float max);
void printbytes(uint32_t var);


void compress40(FILE *input)
{
    A2Methods_T methods = uarray2_methods_blocked;
    Pnm_ppm ppm = get_ppm(input, methods);
    trim_edges(ppm);
    int width = (int)ppm->width;
    int height = (int)ppm->height;

    printf("COMP40 Compressed image format 2\n%i %i\n", width, height);
    for(int r = 0; r < height; r += 2)
    {
        for(int c = 0; c < width; c += 2)
        {
            uint32_t word = get_word_from_chunk(c, r, methods, ppm);
            printbytes(word);

        }
    }
    printf("\n");
}

void decompress40(FILE *input)
{
    (void)input;
}

uint32_t get_word_from_chunk(int col, int row, A2Methods_T methods,
                             Pnm_ppm ppm)
{
    float d = (float)ppm->denominator;
    A2Methods_UArray2 pixmap = ppm->pixels;

    ypp_T tl, tr, bl, br;
    tl = ypp_from_pnm((Pnm_rgb)methods->at(pixmap, col, row), d);
    tr = ypp_from_pnm((Pnm_rgb)methods->at(pixmap, col + 1, row), d);
    bl = ypp_from_pnm((Pnm_rgb)methods->at(pixmap, col, row + 1), d);
    br = ypp_from_pnm((Pnm_rgb)methods->at(pixmap, col + 1, row + 1), d);

    float avg_pb = get_avg_four(tl.pb, tr.pb, bl.pb, br.pb);
    float avg_pr = get_avg_four(tl.pr, tr.pr, bl.pr, br.pr);

    PixSpace_T pix_y = {
      .y1 = tl.y,
      .y2 = tr.y,
      .y3 = bl.y,
      .y4 = br.y
    };
    DCTSpace_T dct = get_DCT_space(pix_y);
    dct = scale_dct_values(dct);

    unsigned idx_pb = Arith40_index_of_chroma(avg_pb);
    unsigned idx_pr = Arith40_index_of_chroma(avg_pr);

    uint32_t word = 0;

    word = Bitpack_newu(word, 9, 23, (uint64_t)round(dct.a));
    word = Bitpack_news(word, 5, 18, (int64_t)round(dct.b));
    word = Bitpack_news(word, 5, 13, (int64_t)round(dct.c));
    word = Bitpack_news(word, 5, 8, (int64_t)round(dct.d));
    word = Bitpack_newu(word, 4, 4, (uint64_t)idx_pb);
    word = Bitpack_newu(word, 4, 0, (uint64_t)idx_pr);

    return (uint32_t)word;
}

DCTSpace_T scale_dct_values(DCTSpace_T dct)
{
    dct.a = bound_to_range(dct.a, 0, 1) * 511;
    dct.b = bound_to_range(dct.b, -0.3, 0.3) * 50;
    dct.c = bound_to_range(dct.c, -0.3, 0.3) * 50;
    dct.d = bound_to_range(dct.d, -0.3, 0.3) * 50;
    return dct;
}

float bound_to_range(float val, float min, float max)
{
    if(val > max)
    {
      val = max;
    }
    if(val < min)
    {
      val = min;
    }
    return val;
}

void printbytes(uint32_t var)
{
    int bpb = 8; /* bits per byte */
    int num_bytes = 4;
    for(int i = 0; i < num_bytes; i++){
      uint64_t curr = Bitpack_getu(var, bpb, num_bytes*bpb-((i+1)*bpb));
      putchar((char)curr);
    }
}
