#ifndef SP_HUFFMAN_H
#define SP_HUFFMAN_H

#include <stddef.h>
#include <stdbool.h>

/* ======================================== */
struct sp_cbb;
struct Huffman;

/* ======================================== */
struct Huffman *
huffman_init(const char *plaintext, size_t len);

int
huffman_free(struct Huffman **);

/* ======================================== */
const char *
huffman_encode(const struct Huffman *,
               const char *plaintext,
               size_t len,
               struct sp_cbb *dest);

bool
huffman_decode(struct Huffman *,
               struct sp_cbb *compressed,
               struct sp_cbb *sink);

/* ======================================== */

#endif
