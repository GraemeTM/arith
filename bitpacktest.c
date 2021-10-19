#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "bitpack.h"

int main()
{
  printf("%d\n", Bitpack_fitsu(33, 5));
  printf("%d\n", Bitpack_fitss(-17, 5));
}
