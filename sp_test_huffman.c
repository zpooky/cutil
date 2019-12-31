#include "sp_test_huffman.h"
#include "sp_huffman.h"

#include <assert.h>
#include <stdio.h>

int
sp_test_huffman(void)
{
  struct Huffman *tree;
  const char *plaintext = "test";
  tree                  = huffman_init("");
  assert(!tree);

  tree = huffman_init(plaintext);
  assert(tree);

  huffman_encode(tree, plaintext);

  huffman_free(&tree);

  return 0;
}
