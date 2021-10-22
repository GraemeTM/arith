/*
 *     compress40.c
 *     by Henry Gray, Graeme McDonough, Oct. 2021
 *     hgray03, gmcdon03
 *     arith
 *
 *     implentation of compression and decompression functions
 *      and accompanying helper functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "compress40.h"
#include "imghandling.h"
#include "bitpack.h"
#include "arith40.h"
#include "pnm.h"
#include "assert.h"
#include "a2plain.h"
#include "color_math.h"
#include <math.h>

/* What to scale b, c, d by to get int representation */
#define BCD_SCALE 50
/* What to scale a by to get int representation */
#define A_SCALE 511
/* Denominator for our output ppm (after decompression) */
#define DENOM 255

/* Standardize the width of our word */
typedef uint32_t word_t;

/* stores codeword information in pairs of the order [width, lsb] */
typedef struct codeword_t {
  float a[2];
  float b[2];
  float c[2];
  float d[2];
  float pb[2];
  float pr[2];
} codeword_t;

/* Define codeword makeup for our current representation */
codeword_t CODEWORD = {
  .a = {9, 23}, .b = {5, 18}, .c = {5, 13},
  .d = {5, 8}, .pb = {4, 4}, .pr = {4, 0}
};

/* Struct to unpack word values into for temporary storage */
typedef struct Unpack_T {
    float a;
    float b;
    float c;
    float d;
    float pb;
    float pr;
} Unpack_T;

/*  Function prototype definition, contracts done with definitions. */
word_t get_word_from_chunk(int col, int row, A2Methods_T methods,
                             Pnm_ppm ppm);

word_t get_word_from_file(FILE *fp);

DCTSpace_T scale_dct_values(DCTSpace_T dct);

float bound_to_range(float val, float min, float max);

void printbytes(word_t var);

Unpack_T unpack_word(word_t word);

void update_pix_map(A2Methods_UArray2 pixmap, PixSpace_T pix_y, float pb_avg,
              float pr_avg, A2Methods_T methods, int col, int row);

uint64_t readbytes(unsigned char c, int idx, uint64_t word);
/* =================================================== */

/* 
 * Purpose: Reads in original ppm, prints our compressed format to stdout
 * Parameters: File pointer to original ppm image 
 * Returns: void
 */ 
void compress40(FILE *input)
{
    A2Methods_T methods = uarray2_methods_plain;
    Pnm_ppm ppm = get_ppm(input, methods);
    trim_edges(ppm);

    int width = (int)ppm->width;
    int height = (int)ppm->height;

    printf("COMP40 Compressed image format 2\n%i %i\n", width, height);
    for(int r = 0; r < height; r += 2)
    {
        for(int c = 0; c < width; c += 2)
        {
            word_t word = get_word_from_chunk(c, r, methods, ppm);
            /* print word in ~big-endian~ */
            printbytes(word);
        }
    }
    Pnm_ppmfree(&ppm);
}

/* 
 * Purpose: Reads in compressed file, prints decompressed ppm to stdout
 * Parameters: File pointer to compressed image format
 * Returns: void
 */ 
void decompress40(FILE *input)
{
    unsigned width, height;
    read_header(input, &width, &height);

    A2Methods_T methods = uarray2_methods_plain;
    Pnm_ppm out = Pnm_ppm_new(width, height, DENOM, methods);

    for(int r = 0; r < (int)height; r += 2)
    {
        for(int c = 0; c < (int)width; c += 2)
        {
            word_t word = get_word_from_file(input);
            Unpack_T unpacked_word = unpack_word(word);
            DCTSpace_T dct = new_dct_t(unpacked_word.a, unpacked_word.b,
                                       unpacked_word.c, unpacked_word.d);
            PixSpace_T pix_y = get_pix_space(dct);
            update_pix_map(out->pixels, pix_y, unpacked_word.pb,
                           unpacked_word.pr, methods, c, r);
        }
    }

    Pnm_ppmwrite(stdout, out);
    Pnm_ppmfree(&out);
}

/* 
 * Purpose: Gets a packed word representation of the 2x2 block of pixels from
 *          a given col, row index
 * Parameters: (col, row) indices, methods struct, and the ppm to read from
 * Returns: packed word_t (uint32_t)
 */
word_t get_word_from_chunk(int col, int row, A2Methods_T methods,
                             Pnm_ppm ppm)
{
    float d = (float)ppm->denominator;
    /* Get pixelmap to work with */
    A2Methods_UArray2 pixmap = ppm->pixels;

    /* Get each of the four pixels in YPP representation */
    ypp_T tl, tr, bl, br;
    tl = ypp_from_pnm((Pnm_rgb)methods->at(pixmap, col, row), d);
    tr = ypp_from_pnm((Pnm_rgb)methods->at(pixmap, col + 1, row), d);
    bl = ypp_from_pnm((Pnm_rgb)methods->at(pixmap, col, row + 1), d);
    br = ypp_from_pnm((Pnm_rgb)methods->at(pixmap, col + 1, row + 1), d);

    /* Get average pb and pr for storage */
    float avg_pb = get_avg_four(tl.pb, tr.pb, bl.pb, br.pb);
    float avg_pr = get_avg_four(tl.pr, tr.pr, bl.pr, br.pr);

    /* Get DCT representation (a, b, c, d) and scale them for packing */
    PixSpace_T pix_y = new_pix_t(tl.y, tr.y, bl.y, br.y);
    DCTSpace_T dct = get_DCT_space(pix_y);
    /* dct now holds SCALED values */
    dct = scale_dct_values(dct);

    /* Get the pb, pr representations for packing */
    unsigned idx_pb = Arith40_index_of_chroma(avg_pb);
    unsigned idx_pr = Arith40_index_of_chroma(avg_pr);

    /* Initialize word, codeword struct */
    uint64_t word = 0;
    codeword_t cw = CODEWORD;

    /* Pack all our values into the word! */
    word = Bitpack_newu(word, cw.a[0], cw.a[1], (uint64_t)round(dct.a));
    word = Bitpack_news(word, cw.b[0], cw.b[1], (int64_t)round(dct.b));
    word = Bitpack_news(word, cw.c[0], cw.c[1], (int64_t)round(dct.c));
    word = Bitpack_news(word, cw.d[0], cw.d[1], (int64_t)round(dct.d));
    word = Bitpack_newu(word, cw.pb[0], cw.pb[1], (uint64_t)idx_pb);
    word = Bitpack_newu(word, cw.pr[0], cw.pr[1], (uint64_t)idx_pr);

    return (word_t)word;
}

/* 
 * Purpose: Reads in a single word from the compressed format
 * Parameters: File pointer to compressed image
 * Returns: Packed 32 bit 
 */
word_t get_word_from_file(FILE *fp)
{
    uint32_t out = 0;
    for(int i = 0; i < 4; i++)
    {
        /* Read in only 32 bits (1byte * 4) */
        out = readbytes(fgetc(fp), i, out);
    }
    return (word_t)out;
}

/* 
 * Purpose: Scale each of the DCT representations according to spec (a, b, c, d)
 * Parameters: DCTSpace_T struct to scale
 * Returns: Modified DCTSpace_T struct
 */
DCTSpace_T scale_dct_values(DCTSpace_T dct)
{
    dct.a = bound_to_range(dct.a, 0, 1) * A_SCALE;
    dct.b = bound_to_range(dct.b, -0.3, 0.3) * BCD_SCALE;
    dct.c = bound_to_range(dct.c, -0.3, 0.3) * BCD_SCALE;
    dct.d = bound_to_range(dct.d, -0.3, 0.3) * BCD_SCALE;
    return dct;
}

/* 
 * Purpose: Make sure that value are within bounds, and mdoify values such that
 *          they are if necessary.
 * Parameters: val to check, min and max values of range.
 * Returns: new value (may be unchanged if already in range)
 */
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

/* 
 * Purpose: Print bytes of the packed word in big-endian order to stdout
 * Parameters: word_t packed word
 * Returns: void
 */ 
void printbytes(word_t var)
{
    int bpb = 8; /* bits per byte */
    int num_bytes = 4;
    for(int i = 0; i < num_bytes; i++){
        uint64_t curr = Bitpack_getu(var, bpb, num_bytes*bpb-((i+1)*bpb));
        putchar((char)curr);
    }
}

/* 
 * Purpose: Packs a byte into a word at a given byte index
 * Parameters: char representation of bytes, int index, and the word to pack
 * Returns: new word value
 */ 
uint64_t readbytes(unsigned char c, int idx, uint64_t word)
{
    int bpb = 8; /* bits per byte */
    int num_bytes = 4;
    return Bitpack_newu(word, bpb, num_bytes*bpb-((idx+1)*bpb), c);
}

/* 
 * Purpose: Unpack a word into a Unpack_T struct for temporary storage
 * Parameters: word_t word to unpack
 * Returns: Unpack_T struct with all expanded values stored.
 */
Unpack_T unpack_word(word_t word)
{
    Unpack_T out;
    codeword_t cw = CODEWORD;

    out.a = Bitpack_getu(word, cw.a[0], cw.a[1]) / (float)A_SCALE;
    out.b = Bitpack_gets(word, cw.b[0], cw.b[1]) / (float)BCD_SCALE;
    out.c = Bitpack_gets(word, cw.c[0], cw.c[1]) / (float)BCD_SCALE;
    out.d = Bitpack_gets(word, cw.d[0], cw.d[1]) / (float)BCD_SCALE;
    out.pb = Arith40_chroma_of_index(Bitpack_getu(word, cw.pb[0], cw.pb[1]));
    out.pr = Arith40_chroma_of_index(Bitpack_getu(word, cw.pr[0], cw.pr[1]));

    return out;
}

/* 
 * Purpose: Populates the output pixelmap for the decompressed image for a given
 *          2x2 chunk
 * Parameters: pixmap to modify, PixSpace_T struct from which to read, the 
 *              Pb and Pr average values, methods, column and row indices
 * Returns: void (just updating the passed pixmap)
 */
void update_pix_map(A2Methods_UArray2 pixmap, PixSpace_T pix_y, float pb_avg,
              float pr_avg, A2Methods_T methods, int col, int row)
{
    /* For every 4  */
    float arr[4] = {pix_y.y1, pix_y.y2, pix_y.y3, pix_y.y4};
    for(int i = 0; i < 4; i++)
    {
        /* Get compressed ypp representation */
        ypp_T cypp = {
            .y = arr[i],
            .pb = pb_avg,
            .pr = pr_avg
        };

        /* Convert to rgb */
        rgb_T crgb = ypp_to_rgb(cypp);

        /* Initialize new pixel for storage */
        Pnm_rgb new_pix = malloc(sizeof(*new_pix));
        assert(new_pix);

        /* Assign bounded pixel values */
        new_pix->red = (unsigned)bound_to_range(crgb.red * DENOM, 0, 255);
        new_pix->green = (unsigned)bound_to_range(crgb.green * DENOM, 0, 255);
        new_pix->blue = (unsigned)bound_to_range(crgb.blue * DENOM, 0, 255);

        /* Vary the position within the 2x2 chunk of pixels */
        int x_off = i % 2;
        int y_off = i > 1;

        /* put new pixel value into output pixmap */
        *((Pnm_rgb)methods->at(pixmap, col + x_off, row + y_off)) = *new_pix;
        free(new_pix);
    }
}
