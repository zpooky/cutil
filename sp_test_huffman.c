#include "sp_test_huffman.h"
#include "sp_huffman.h"
#include "sp_cbb.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

int
sp_test_huffman(void)
{
  struct Huffman *tree;
  struct sp_cbb *dest;
  const char *plaintext = "test";

  dest = sp_cbb_init(1024);
  tree = huffman_init(plaintext, strlen(plaintext));
  assert(tree);

  huffman_encode(tree, plaintext, strlen(plaintext), dest);

  sp_cbb_clear(dest);
  sp_cbb_free(&dest);
  huffman_free(&tree);

  return 0;
}
