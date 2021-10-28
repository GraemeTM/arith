/*
 *     bitpack.c
 *     by Henry Gray, Graeme McDonough, Oct. 2021
 *     hgray03, gmcdon03
 *     arith
 *
 *     Implementation of bitpacking functions
 */

#include <stdbool.h>
#include <stdint.h>
#include "math.h"
#include "assert.h"
#include "except.h"
#include <stdio.h>

#define WORD_W 64

uint64_t lshiftu(uint64_t word, int shift);
uint64_t rshiftu(uint64_t word, int shift);
int64_t lshifts(int64_t word, int shift);
int64_t rshifts(int64_t word, int shift);

bool Bitpack_fitsu(uint64_t n, unsigned width)
{
    return n <= (uint64_t)pow(2, width) - 1;
}

bool Bitpack_fitss(int64_t n, unsigned width)
{
    return n >= -1 * (int64_t)pow(2, width-1)
        && n <= (int64_t)pow(2, width-1) - 1;
}

/* Get unsigned int from the word */
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
    assert(width <= WORD_W);
    assert(width + lsb <= WORD_W);

    uint64_t bitmask = ~0;
    bitmask = rshiftu(bitmask, (WORD_W - width));
    bitmask = lshiftu(bitmask, lsb);

    uint64_t val = bitmask & word;
    val = rshiftu(val, lsb);
    return val;
}

int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
    assert(width <= WORD_W);
    assert(width + lsb <= WORD_W);

    uint64_t bitmask = ~0;
    bitmask = rshiftu(bitmask, (WORD_W - width)); 
    bitmask = lshiftu(bitmask, lsb);

    int64_t val = bitmask & word;

    val = lshifts(val, (WORD_W - (width + lsb)));
    val = rshifts(val, (WORD_W - width));

    return val;
}


uint64_t
Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, uint64_t value)
{
    assert(width <= WORD_W);
    assert(width + lsb <= WORD_W);

    Except_T Bitpack_Overflow = {"Overflow packing bits"};
    if(!Bitpack_fitsu(value, width))
    {
        RAISE(Bitpack_Overflow);
    }

    /* Get 0s in range to nullify exisiting value */
    uint64_t mask_h = ~0;
    mask_h = lshiftu(mask_h, (width + lsb));

    uint64_t mask_l = ~0;
    mask_l = rshiftu(mask_l, 64 - lsb);

    uint64_t mask = mask_l | mask_h;

    /* nullify value first*/
    word &= mask;
    /* put in new value */
    value = lshiftu(value, lsb);


    word = word | value;

    return word;
}


uint64_t
Bitpack_news(uint64_t word, unsigned width, unsigned lsb, int64_t value)
{
    assert(width <= WORD_W);
    assert(width + lsb <= WORD_W);

    Except_T Bitpack_Overflow = {"Overflow packing bits"};
    if (!Bitpack_fitss(value, width))
    {
        fprintf(stderr, "%u, %lu\n", width, value);
        RAISE(Bitpack_Overflow);
    }

    /* Get 0s in range to nullify exisiting value */
    uint64_t mask_h = ~0;
    mask_h = lshiftu(mask_h, width + lsb);

    uint64_t mask_l = ~0;
    mask_l = rshiftu(mask_l, 64 - lsb);

    uint64_t mask = mask_l | mask_h;

    /* nullify value first*/

    word &= mask;

    /* put in new value */
    uint64_t val = (uint64_t)value;
    val = lshiftu(val, WORD_W - width);
    val = rshiftu(val, WORD_W - width - lsb);

    word |= val;

    return word;
}


/* All functions just check for shifts of 64 and account for this */
uint64_t lshiftu(uint64_t word, int shift)
{
    if (shift == 64) {
        word = word << 1;
        word = word << (WORD_W - 1);
    } else {
        word = word << shift;
    }
    return word;
}

uint64_t rshiftu(uint64_t word, int shift)
{
    if (shift == 64) {
        word = word >> 1;
        word = word >> (WORD_W - 1);
    } else {
        word = word >> shift;
    }
    return word;
}

int64_t lshifts(int64_t word, int shift)
{
    if (shift == 64) {
        word = word << 1;
        word = word << (WORD_W - 1);
    } else {
        word = word << shift;
    }
    return word;
}

int64_t rshifts(int64_t word, int shift)
{
    if (shift == 64) {
        word = word >> 1;
        word = word >> (WORD_W - 1);
    } else {
        word = word >> shift;
    }
    return word;
}

