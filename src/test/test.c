#include <stdlib.h>

#include "sp_test_bst.h"
#include "sp_test_heap.h"
#include "sp_test_heap_copy.h"
#include "sp_test_huffman.h"
#include "sp_test_perfect_hash_map.h"
#include "sp_test_hash.h"
#include "sp_test_hashset.h"

int
main()
{
  srand(0);
  sp_test_hashset();
  sp_test_bst();
  sp_test_heap();
  sp_test_huffman();
  sp_test_hash();
  sp_test_heap_copy();
  /* sp_test_perfect_hash_map(); */
}
