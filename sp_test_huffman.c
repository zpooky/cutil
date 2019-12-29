#include "sp_test_huffman.h"
#include "sp_huffman.h"

#include <assert.h>
#include <stdio.h>

int
sp_test_huffman(void)
{
  struct Huffman *tree;
  tree = huffman_init("");
  assert(!tree);

  tree = huffman_init("test");
  assert(tree);

  huffman_free(&tree);

  return 0;
}
