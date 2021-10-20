#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "bitpack.h"

void printbytes(uint32_t var)
{
    int bpb = 8; /* bits per byte */
    int num_bytes = 4;
    for(int i = 0; i < num_bytes; i++){
      uint64_t curr = Bitpack_getu(var, bpb, num_bytes*bpb-((i+1)*bpb));
      putchar((char)curr);
    }
}

void printbytes_lil(void *p, unsigned int len)
{
    unsigned int i;
    unsigned char *cp = (unsigned char *)p;
    for (i = 0; i < len; i++) {
        printf("%02X", *cp++);
    }
    printf("\n");
}

int main()
{

  int pos = 0x424141;


  printf("The bytes in memory for signed integer %d are ", pos);
  printbytes_lil(&pos, sizeof(pos));


  printf("---------------\n");
  printf("The bytes in memory for signed integer %d are ", pos);
  printbytes(pos);


  // printf("%d\n", Bitpack_fitsu(33, 5));
  // printf("%d\n", Bitpack_fitss(-17, 5));
  // printf("%lu\n", Bitpack_getu(0x2f4, 6, 3));
  // printf("%li\n", Bitpack_gets(0x3f4, 6, 2));
  //
  // printf("YAY: %d\n", Bitpack_getu(Bitpack_newu(5, 4, 3, 2), 4, 3) == 2);
  // printf("YAY: %d\n", Bitpack_gets(Bitpack_news(5, 4, 3, 2), 4, 3) == 2);
  // printf("YAY: %d\n",
  //   Bitpack_getu(Bitpack_newu(5, 4, 3, 2), 4, 8) == Bitpack_getu(5, 4, 8));
  // printf("YAY: %d\n",
  //   Bitpack_getu(Bitpack_newu(5, 4, 3, 2), 4, 8) == Bitpack_getu(5, 4, 8));
  //
  // uint64_t input = 0x2f4;
  // uint64_t val = 4;
  //
  // // get word
  // uint64_t new = Bitpack_news(input, 4, 2, val);
  // printf("After packing word with 4: %lx\n", new);
  //
  // // get value
  // printf("Getting out value: %lu\n", Bitpack_gets(new, 4, 2));
}
