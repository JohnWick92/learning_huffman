#ifndef UTF8_FREQUENCY_H
#define UTF8_FREQUENCY_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/**
 * Total number of valid Unicode code points (U+0000 through U+10FFFF).
 * Allocating a direct-mapped array of this size costs ~4.45 MB (1,114,112 * 4 bytes)
 * and guarantees O(1) worst-case lookup and insertion times.
 */
#define MAX_UNICODE 0x110000

/**
 * 64 KB stack-allocated I/O read buffer.
 * Batching reads via fread() amortizes syscall overhead and keeps data hot in L1/L2 CPU cache.
 */
#define UTF8_BUF_SIZE 65536

/**
 * @brief Builds a Unicode code point frequency table from a binary-mode UTF-8 stream.
 *
 * Uses branchless 32-entry lookup tables indexed by the top 5 bits of the leading byte
 * and a stateful streaming decoder to seamlessly handle multi-byte sequences split
 * across 64 KB chunk boundaries.
 *
 * @param fp Pointer to an open FILE stream (must be opened in binary mode, e.g., "rb").
 *           The caller retains ownership and is responsible for closing the stream.
 * @return Dynamically allocated array of size MAX_UNICODE containing the frequency of
 *         each Unicode code point, or NULL if `fp` is invalid or memory allocation fails.
 *         The caller is responsible for calling free() on the returned pointer.
 */
static inline uint32_t *build_utf8_frequency_table(FILE *fp) {
    if (!fp) return NULL;

    // Zero-initialized direct-mapped table covering the entire Unicode codespace
    uint32_t *freq = (uint32_t *)calloc(MAX_UNICODE, sizeof(uint32_t));
    if (!freq) return NULL;

    /**
     * Branchless UTF-8 Sequence Length Lookup Table:
     * Shifting the leading byte right by 3 (`b >> 3`) isolates its 5 most significant bits,
     * mapping all 256 possible byte values onto 32 indices (0..31):
     *   - 0b00000..0b01111 (0..15)  -> 0xxxxxxx: 1-byte ASCII sequence
     *   - 0b10000..0b10111 (16..23) -> 10xxxxxx: Unexpected continuation byte (invalid start -> 0)
     *   - 0b11000..0b11011 (24..27) -> 110xxxxx: 2-byte sequence
     *   - 0b11100..0b11101 (28..29) -> 1110xxxx: 3-byte sequence
     *   - 0b11110          (30)     -> 11110xxx: 4-byte sequence
     *   - 0b11111          (31)     -> 11111xxx: Invalid UTF-8 prefix (0)
     */
    static const uint8_t LEN_TAB[] = {
        1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
        0,0,0,0,0,0,0,0,
        2,2,2,2,
        3,3,
        4,
        0
    };

    /**
     * Branchless Payload Bitmask Lookup Table:
     * Strips the UTF-8 length prefix bits from the leading byte to extract the initial
     * code point payload bits:
     *   - 1-byte (0xxxxxxx) -> 0x7F (0b01111111, keeps 7 bits)
     *   - 2-byte (110xxxxx) -> 0x1F (0b00011111, keeps 5 bits)
     *   - 3-byte (1110xxxx) -> 0x0F (0b00001111, keeps 4 bits)
     *   - 4-byte (11110xxx) -> 0x07 (0b00000111, keeps 3 bits)
     */
    static const uint8_t MASK_TAB[] = {
        0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F, 0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x1F,0x1F,0x1F,0x1F,
        0x0F,0x0F,
        0x07,
        0x00
    };

    uint8_t buf[UTF8_BUF_SIZE];
    size_t bytes_read;

    // Decoder state preserved across fread() chunk boundaries
    uint32_t codepoint = 0; // Code point accumulator
    int remaining = 0;      // Number of continuation bytes still expected (0 = awaiting new char)

    while ((bytes_read = fread(buf, 1, sizeof(buf), fp)) > 0) {
        for (size_t i = 0; i < bytes_read; i++) {
            uint8_t b = buf[i];

            if (remaining == 0) {
                // State 0: Expecting a leading byte
                uint8_t idx = b >> 3;
                uint8_t len = LEN_TAB[idx];

                // Skip stray continuation bytes (10xxxxxx) or invalid prefixes (0xF8..0xFF)
                if (len == 0) continue;

                codepoint = b & MASK_TAB[idx];
                remaining = len - 1;
            } else {
                // State > 0: Expecting a continuation byte matching 10xxxxxx (0x80..0xBF)
                if ((b & 0xC0) != 0x80) {
                    // Malformed/truncated multi-byte sequence detected:
                    // Discard partial code point, reset state, and rewind index (`i--`)
                    // so the current byte is re-evaluated as a potential new leading byte.
                    remaining = 0;
                    i--;
                    continue;
                }

                // Shift accumulated bits left by 6 and append the 6 payload bits (00xxxxxx)
                codepoint = (codepoint << 6) | (b & 0x3F);
                remaining--;
            }

            // Once all expected bytes are assembled, record the code point if within valid bounds
            if (remaining == 0 && codepoint < MAX_UNICODE) {
                freq[codepoint]++;
            }
        }
    }

    return freq;
}

#endif // UTF_FREQUENCY_H
