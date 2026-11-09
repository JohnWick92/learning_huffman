#include <stdio.h>
#include <stdlib.h>
#include "../inc/file.h"

int main(void) {
  printf("This should read a file and compress the archive using Huffman algorithm\n");
  FILE* fptr;
  unsigned long frequency[MAX_ASCII_CHARACTER];

  get_ascii_char_frequency(fptr, frequency);
  return EXIT_SUCCESS;
}
