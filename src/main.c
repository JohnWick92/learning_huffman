#include <stdio.h>
#include <stdlib.h>
#include "../inc/file.h"

int main(int argc, char *argv[]) {
  FILE*         fptr = NULL;
  unsigned long frequency[ASCII_SIZE] = {0};

  if (argc != 2) {
    fprintf(stderr, "Usage %s <filename>\n", argv[0]);
    return EXIT_FAILURE;
  }

  fptr = fopen(argv[1], "rb");
  if(!fptr) {
    perror("Error opening the file");
    return EXIT_FAILURE;
  }

  printf("This should read a file and compress the archive using Huffman algorithm\n");
  if (get_ascii_char_frequency(fptr, frequency) != 0) {
    // TODO: Need to improve this!
    printf("Something went wrong\n");
  }

  fclose(fptr);
  return EXIT_SUCCESS;
}
