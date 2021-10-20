#include <stdbool.h>
#include <stdint.h>
#include "math.h"
#include "assert.h"
#include "except.h"
#include <stdio.h>

#define WORD_W 64



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
    bitmask = bitmask >> (WORD_W - width) << lsb;

    uint64_t val = bitmask & word;
    val = val >> lsb;
    return val;
}

int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
    assert(width <= WORD_W);
    assert(width + lsb <= WORD_W);

    uint64_t bitmask = ~0;
    bitmask = bitmask >> (WORD_W- width) << lsb;

    int64_t val = bitmask & word;

    val = val << (WORD_W - (width + lsb));
    val = val >> (WORD_W - width);

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
        fprintf(stderr, "%lu, %u\n", value, width);
        RAISE(Bitpack_Overflow);
    }

    /* Get 0s in range to nullify exisiting value */
    uint64_t mask_h = ~0;
    mask_h = mask_h << (width + lsb);

    uint64_t mask_l = ~0;
    mask_l = mask_l >> (64 - lsb);

    uint64_t mask = mask_l | mask_h;

    /* nullify value first*/
    word &= mask;
    /* put in new value */
    value = value << lsb;


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

        RAISE(Bitpack_Overflow);
    }

    /* Get 0s in range to nullify exisiting value */
    uint64_t mask_h = ~0;
    mask_h = mask_h << (width + lsb);

    uint64_t mask_l = ~0;
    mask_l = mask_l >> (64 - lsb);

    uint64_t mask = mask_l | mask_h;

    /* nullify value first*/

    word &= mask;

    /* put in new value */
    uint64_t val = (uint64_t)value;
    val = val << (WORD_W - width);
    val = val >> (WORD_W - width - lsb);

    word |= val;

    return word;
}
