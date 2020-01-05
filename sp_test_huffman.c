#include "sp_test_huffman.h"
#include "sp_huffman.h"
#include "sp_cbb.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>

int
sp_test_huffman(void)
{
  struct Huffman *tree;
  struct sp_cbb *compressed, *uncompressed;
  const char *plaintext = "test";
  const size_t psize    = strlen(plaintext) + 1;
  char tmp[256]         = {0};

  compressed   = sp_cbb_init(1024);
  uncompressed = sp_cbb_init(1024);

  tree = huffman_init(plaintext, psize);
  assert(tree);

  huffman_encode(tree, plaintext, psize, compressed);
  huffman_decode(tree, compressed, uncompressed);

  assert(sp_cbb_is_empty(compressed));
  assert(!sp_cbb_is_empty(uncompressed));
  /* assert(sp_cbb_length(uncompressed) == psize); */

sp_cbb_pop_front(uncompressed, tmp, sizeof(tmp));
  /* assert( == psize); */
  printf("|%s|\n", tmp);
  assert(strcmp(plaintext, tmp) == 0);

  sp_cbb_free(&compressed);

  sp_cbb_clear(uncompressed);
  sp_cbb_free(&uncompressed);
  huffman_free(&tree);

  return 0;
}
