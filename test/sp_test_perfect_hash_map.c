#include "sp_test_perfect_hash_map.h"

#include <assert.h>
#include <stdio.h>

#include "sp_perfect_hash_map.h"
#include "sp_hash.h"
#include "sp_util.h"

static uint32_t
sp_test_hash(uint32_t seed, uint32_t *key, void *closure)
{
  uint32_t hash;
  (void)closure;

  assert(key);

  /* printf("key: %u\n", *key); */

  hash = sp_hash_fnv1a0(&seed, sizeof(seed));
  return sp_hash_fnv1a(hash, key, sizeof(*key));
}

int
sp_test_perfect_hash_map(void)
{
  size_t MAX;
  for (MAX = 9; MAX < 1024; ++MAX) {
    printf("==================\n");
    struct sp_phash_map *dict;
    uint32_t i;
    struct sp_pair b[MAX];
    uint32_t storage[MAX];
    for (i = 0; i < MAX; ++i) {
      storage[i] = i;

      b[i].first  = storage + i;
      b[i].second = storage + i;
    }

    dict = sp_phash_map_init(b, MAX, (sp_phash_map_hash_cb)sp_test_hash, NULL);

    for (i = 0; i < MAX; ++i) {
      uint32_t *value = sp_phash_map_lookup(dict, &i);
      printf("%u: %u\n", i, *value);
      assert(value);
      assert(*value == i);
    }

    sp_phash_map_free(&dict);
  }
  return 0;
}
