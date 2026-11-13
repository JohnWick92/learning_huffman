#include <stdio.h>
#include <stdlib.h>
#include "../inc/utf8_frequency.h"

int main(int argc, char *argv[]) {
  FILE*         fptr = NULL;
  uint32_t*     frequency_table = {0};


  if (argc != 2) {
    fprintf(stderr, "Usage %s <filename>\n", argv[0]);
    return EXIT_FAILURE;
  }

  fptr = fopen(argv[1], "rb");
  if(!fptr) {
    perror("Error opening the file");
    return EXIT_FAILURE;
  }

  frequency_table = build_utf8_frequency_table(fptr);

  fclose(fptr);
  return EXIT_SUCCESS;
}
