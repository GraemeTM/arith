#include <stdbool.h>
#include <stdint.h>
#include "math.h"
#include <stdio.h>

bool Bitpack_fitsu(uint64_t n, unsigned width)
{
  return n <= (uint64_t)pow(2, width) - 1;
}

bool Bitpack_fitss(int64_t n, unsigned width)
{
  return n >= -1 * (int64_t)pow(2, width-1)
      && n <= (int64_t)pow(2, width-1) - 1;
}
