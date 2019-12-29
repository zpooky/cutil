#ifndef SP_INTERV_HUFFMAN_H
#define SP_INTERV_HUFFMAN_H

/* ======================================== */
struct Huffman;

/* ======================================== */
struct Huffman *
huffman_init(const char *plaintext);

int
huffman_free(struct Huffman **);

/* ======================================== */
void
huffman_encode(const struct Huffman *, const char *plaintext);

void
huffman_decode(const struct Huffman *, const char *compressed);

/* ======================================== */

#endif
