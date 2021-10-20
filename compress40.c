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

#define BCD_SCALE 50
#define A_SCALE 511
#define DENOM 255


typedef struct Unpack_T {
    float a;
    float b;
    float c;
    float d;
    float pb;
    float pr;
} Unpack_T;

void printbytes_hex(uint32_t var)
{
    int bpb = 8; /* bits per byte */
    int num_bytes = 4;
    for(int i = 0; i < num_bytes; i++){
      uint64_t curr = Bitpack_getu(var, bpb, num_bytes*bpb-((i+1)*bpb));
      printf("%02lX", curr);
    }
    printf("\n");
}

uint32_t reverse_endian_32(uint32_t var)
{
  int bpb = 8; /* bits per byte */
  int num_bytes = 4;
  uint64_t arr[4];
  uint64_t word = 0;
  for(int i = 0; i < num_bytes; i++){
    uint64_t curr = Bitpack_getu(var, bpb, num_bytes*bpb-((i+1)*bpb));
    arr[i] = curr;
  }
  for(int i = 0; i < num_bytes; i++){
    uint64_t curr = arr[num_bytes-i-1];
    word = Bitpack_newu(word, bpb, num_bytes*bpb-((i+1)*bpb), curr);
  }
  return (uint32_t)word;
}


uint32_t get_word_from_chunk(int col, int row, A2Methods_T methods,
                             Pnm_ppm ppm);
uint32_t get_word_from_file(FILE *fp);
DCTSpace_T scale_dct_values(DCTSpace_T dct);
float bound_to_range(float val, float min, float max);
void printbytes(uint32_t var);
Unpack_T unpack_word(uint32_t word);
void update_pix_map(A2Methods_UArray2 pixmap, PixSpace_T pix_y, float pb_avg,
              float pr_avg, A2Methods_T methods, int col, int row);
uint64_t readbytes(unsigned char c, int idx, uint64_t word);


void print_map(int i, int j, A2Methods_UArray2 array2,
                          A2Methods_Object *ptr, void *cl)
{
    Pnm_rgb curr = (Pnm_rgb)ptr;
    printf("%u %u %u\n", curr->red, curr->blue, curr->green);
    (void)i;
    (void)j;
    (void)cl;
    (void)array2;
}



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
            //printf("%X\n", word);
            //printbytes(word);
            (void)word;
        }
    }
}


void decompress40(FILE *input)
{
    unsigned height, width;
    int read = fscanf(input, "COMP40 Compressed image format 2\n%u %u",
                      &width, &height);
    assert(read == 2);
    int c = getc(input);
    assert(c == '\n');
    Pnm_ppm out = malloc(sizeof(*out));
    assert(out);
    A2Methods_T methods = uarray2_methods_blocked;
    out->width = width;
    out->height = height;
    out->denominator = DENOM;
    out->methods = methods;
    Pnm_rgb new_pix = malloc(sizeof(*new_pix));
    out->pixels = methods->new(width, height, sizeof(*new_pix));
    free(new_pix);

    for(int r = 0; r < (int)height; r += 2)
    {
        for(int c = 0; c < (int)width; c += 2)
        {
            uint32_t word = get_word_from_file(input);
            // printf("%X\n", word);
            Unpack_T unpacked_word = unpack_word(word);
            DCTSpace_T dct = {
              .a = unpacked_word.a,
              .b = unpacked_word.b,
              .c = unpacked_word.c,
              .d = unpacked_word.d,
            };
            PixSpace_T pix_y = get_pix_space(dct);
            update_pix_map(out->pixels, pix_y, unpacked_word.pb,
                           unpacked_word.pr, methods, c, r);
        }
    }

    printf("P6\n");
    printf("%u %u\n", width, height);
    printf("%d\n", out->denominator);
    for(int r = 0; r < (int)height; r++)
    {
        for(int c = 0; c < (int)height; c++)
        {
            Pnm_rgb curr = ((Pnm_rgb)methods->at(out->pixels, c, r));
            printf("%c%c%c", curr->red, curr->green, curr->blue);

        }
        fprintf(stderr, "%d\n", r);
        printf("\n");
    }
    //Pnm_ppmwrite(stdout, out);
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
    // printf("c: %f, %f, %f, %f, %f, %f\n", dct.a, dct.b, dct.c, dct.d, avg_pb, avg_pr);
    dct = scale_dct_values(dct);

    unsigned idx_pb = Arith40_index_of_chroma(avg_pb);
    unsigned idx_pr = Arith40_index_of_chroma(avg_pr);

    uint64_t word = 0;


    word = Bitpack_newu(word, 9, 23, (uint64_t)round(dct.a));
    //printf("a: %ld\n", Bitpack_getu(word, 9, 23));
    word = Bitpack_news(word, 5, 18, (int64_t)round(dct.b));
    word = Bitpack_news(word, 5, 13, (int64_t)round(dct.c));
    word = Bitpack_news(word, 5, 8, (int64_t)round(dct.d));
    word = Bitpack_newu(word, 4, 4, (uint64_t)idx_pb);
    word = Bitpack_newu(word, 4, 0, (uint64_t)idx_pr);
    // printf("out: %lu, %li, %li, %li, %lu, %lu\n\n", Bitpack_getu(word, 9, 23),
    //       Bitpack_gets(word, 5, 18), Bitpack_gets(word, 5, 13),
    //       Bitpack_gets(word, 5, 8), Bitpack_getu(word, 4, 4),
    //       Bitpack_getu(word, 4, 0));

    return (uint32_t)word;
}

uint32_t get_word_from_file(FILE *fp)
{
    uint64_t out = 0;
    for(int i = 0; i < 4; i++)
    {
        out = readbytes(fgetc(fp), i, out);
    }
    return (uint32_t)out;
}

DCTSpace_T scale_dct_values(DCTSpace_T dct)
{
    dct.a = bound_to_range(dct.a, 0, 1) * A_SCALE;
    dct.b = bound_to_range(dct.b, -0.3, 0.3) * BCD_SCALE;
    dct.c = bound_to_range(dct.c, -0.3, 0.3) * BCD_SCALE;
    dct.d = bound_to_range(dct.d, -0.3, 0.3) * BCD_SCALE;
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

uint64_t readbytes(unsigned char c, int idx, uint64_t word)
{
    int bpb = 8; /* bits per byte */
    int num_bytes = 4;
    return Bitpack_newu(word, bpb, num_bytes*bpb-((idx+1)*bpb), c);

}

Unpack_T unpack_word(uint32_t word)
{
    Unpack_T out;

    out.a = Bitpack_getu(word, 9, 23) / (float)A_SCALE;
    out.b = Bitpack_gets(word, 5, 18) / (float)BCD_SCALE;
    out.c = Bitpack_gets(word, 5, 13) / (float)BCD_SCALE;
    out.d = Bitpack_gets(word, 5, 8) / (float)BCD_SCALE;
    out.pb = Arith40_chroma_of_index(Bitpack_getu(word, 4, 4));
    out.pr = Arith40_chroma_of_index(Bitpack_getu(word, 4, 0));


    return out;
}

void update_pix_map(A2Methods_UArray2 pixmap, PixSpace_T pix_y, float pb_avg,
              float pr_avg, A2Methods_T methods, int col, int row)
{
    float arr[4] = {pix_y.y1, pix_y.y2, pix_y.y3, pix_y.y4};
    for(int i = 0; i < 4; i++)
    {
        ypp_T cypp = {
            .y = arr[i],
            .pb = pb_avg,
            .pr = pr_avg
        };
        rgb_T crgb = ypp_to_rgb(cypp);

        Pnm_rgb new_pix = malloc(sizeof(*new_pix));
        new_pix->red = (unsigned)bound_to_range(crgb.red * DENOM, 0, 255);
        new_pix->green = (unsigned)bound_to_range(crgb.green * DENOM, 0, 255);
        new_pix->blue = (unsigned)bound_to_range(crgb.blue * DENOM, 0, 255);
        // printf("%u %u %u\n", new_pix->red, new_pix->green, new_pix->blue);
        int x_off = i % 2;
        int y_off = i > 1;
        *(Pnm_rgb)methods->at(pixmap, col + x_off, row + y_off) = *new_pix;
    }
}
