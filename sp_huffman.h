#ifndef SP_INTERV_HUFFMAN_H
#define SP_INTERV_HUFFMAN_H

#include <stddef.h>

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

void
huffman_decode(const struct Huffman *, const char *compressed);

/* ======================================== */

#endif
