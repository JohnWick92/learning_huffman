#ifndef COUNT_ASCII_H
#define COUNT_ASCII_H

#include <stdio.h>

#define ASCII_SIZE 128

/**
 * Reads the whole file pointed to by fp and accumulates, in freq[],
 * how many times each ASCII character (0x00 to 0x7F) appears.
 *
 * @param fptr    file already opened in binary mode ("rb"). Not closed here.
 * @param frequency  array of at least ASCII_SIZE positions. Must be zeroed by
 *              the caller; the function only increments the counters.
 * @return      EXIT_SUCCESS on success; EXIT_FAILURE if a read error
 *              occurred (ferror).
 */
int get_ascii_char_frequency(FILE* fptr, unsigned long frequency[ASCII_SIZE]);

#endif
