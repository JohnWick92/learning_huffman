#include "../inc/file.h"
#include <stdlib.h>

int get_ascii_char_frequency(FILE* fptr, unsigned long frequency[ASCII_SIZE]) {
  if (!fptr) {
    return EXIT_FAILURE;
  }
  size_t        readed_charachter = {0};
  unsigned char buffer[READER_BUFFER] = {0};

  while ((readed_charachter = fread(buffer, sizeof(*buffer), ARRAY_LENGTH(buffer), fptr)) > 0) {
    for (size_t i = 0; i < readed_charachter; i++) {
      frequency[buffer[i]]++;
    }
  }

    for (size_t i = 0; i < ASCII_SIZE; i++) {
    // If the character is printable (between space 32 and tilde 126), print the character.
    // Otherwise (control characters), print the numeric ASCII code in brackets.
    // Using %hhd matches the (char) cast type, avoiding compiler format warnings in a single ternary.
    printf((i >= 32 && i <= 126) ? "'%c': %lu\n" : "[%hhd]: %lu\n", (char)i, frequency[i]);
  }
  return EXIT_SUCCESS;
}
