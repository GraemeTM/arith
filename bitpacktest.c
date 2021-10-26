#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "bitpack.h"

void printbytes(uint32_t var)
{
    int bpb = 8; /* bits per byte */
    int num_bytes = 4;
    for (int i = 0; i < num_bytes; i++) {
      uint64_t curr = Bitpack_getu(var, bpb, num_bytes*bpb-((i+1)*bpb));
      putchar((char)curr);
    }
    printf("\n");
}

uint32_t reverse_endian_32(uint32_t var)
{
  int bpb = 8; /* bits per byte */
  int num_bytes = 4;
  uint64_t arr[4];
  uint64_t word = 0;
  for (int i = 0; i < num_bytes; i++) {
    uint64_t curr = Bitpack_getu(var, bpb, num_bytes*bpb-((i+1)*bpb));
    arr[i] = curr;
  }
  for (int i = 0; i < num_bytes; i++) {
    uint64_t curr = arr[num_bytes-i-1];
    word = Bitpack_newu(word, bpb, num_bytes*bpb-((i+1)*bpb), curr);
  }
  return (uint32_t)word;
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
    uint64_t word = 0;
    // int values[16] = {-12, 3, 3, -6, -1, 0, 1, 4, 12, -2, 61, 0, 1, -1, -7, -7};
    //int values[16] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
    //int values[16] = {-8, 0, -8, 0, -8, 0, -8, 0, -8, 0, -8, 0, -8, 0, -8, 0};
    int num_val = 16;
    // int codeword[32] = {
    //   5, 3, 3, 5, 4, 4, 3, 5, 5, 3, 7, 1, 3, 5, 4, 4,
    //   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    // };
    int codeword[32] = {
      7, 1, 7, 1, 7, 1, 7, 1, 7, 1, 7, 1, 7, 1, 7, 1,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    int values[16] = {-64, 0, -63, 0, -62, 0, -61, 0, -60, 0, -59, 0, -58, 0, -57, 0};
    int sum = 0;
    for (int i = 0; i < num_val; i++)
    {

      sum += codeword[i];
      codeword[i+num_val] = 64 - sum;
      word = Bitpack_news(word, codeword[i], codeword[i+num_val], values[i]);
    }
    int all_good = 1;
    for (int i = 0; i < num_val; i++)
    {
        int val = Bitpack_gets(word, codeword[i], codeword[i+num_val]);
        all_good = val == values[i];
        if (!all_good)
        {
           fprintf(stderr, "Damb\n");
           printf("word: %lX\n", word);
           exit(EXIT_FAILURE);
        }
    }
    printf("word: %lX\n", word);
    fprintf(stderr, "hells yea\n");



//  int pos = 0x45444342;


  // printf("---------------\n");
  // printf("The bytes in memory for signed integer %d are\n", pos);
  // printbytes(pos);
  // uint32_t newpos = reverse_endian_32(pos);
  // printbytes(newpos);


  // printf("%d\n", Bitpack_fitsu(33, 5));
  // printf("%d\n", Bitpack_fitss(-17, 5));
  // printf("%lu\n", Bitpack_getu(0x2f4, 6, 3));
  // printf("%li\n", Bitpack_gets(0x3f4, 6, 2));
  //
  // uint64_t word = 0;
  // word = Bitpack_newu(word, 9, 23, 163);
  // printf("test: %lX\n", word);
  // printf("hmm: %ld\n", Bitpack_getu(word, 9, 23));
  // word = Bitpack_newu(word, 5, 18, 1);
  // printf("test: %lX\n", word);
  // printf("hmm: %ld\n", Bitpack_getu(word, 9, 23));
  //
  // word = Bitpack_news(word, 5, 13, -5);
  // printf("test: %lX\n", word);
  // printf("hmm: %ld\n", Bitpack_getu(word, 9, 23));
  // word = Bitpack_news(word, 5, 8, 4);
  // printf("test: %lX\n", word);
  // printf("hmm: %ld\n", Bitpack_getu(word, 9, 23));
  //
  // word = Bitpack_newu(word, 4, 4, 6);
  // printf("test: %lX\n", word);
  // printf("hmm: %ld\n", Bitpack_getu(word, 9, 23));
  // word = Bitpack_newu(word, 4, 0, 7);
  // printf("test: %lX\n", word);
  // printf("hmm: %ld\n", Bitpack_getu(word, 9, 23));
  // printf("hmm: %ld\n", Bitpack_gets(word, 5, 18));
  // printf("hmm: %ld\n", Bitpack_gets(word, 5, 13));
  // printf("hmm: %ld\n", Bitpack_gets(word, 5, 8));
  // printf("hmm: %ld\n", Bitpack_getu(word, 4, 4));
  // printf("hmm: %ld\n", Bitpack_getu(word, 4, 0));



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
