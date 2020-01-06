#include "sp_test_perfect_hash_map.h"

#include <assert.h>

#include "sp_perfect_hash_map.h"
#include "sp_hash.h"
#include "sp_util.h"

static uint32_t
sp_test_hash(uint32_t seed, uint32_t *key, void *closure)
{
  uint32_t hash;
  (void)closure;

  hash = sp_hash_fnv1a0(&seed, sizeof(seed));
  return sp_hash_fnv1a(hash, key, sizeof(*key));
}

int
sp_test_perfect_hash_map(void)
{
  struct sp_phash_map *dict;
  const size_t MAX = 1024;
  uint32_t i;
  struct sp_pair buffer[MAX];
  uint32_t storage[MAX];
  for (i = 0; i < MAX; ++i) {
    storage[i] = i;

    buffer[i].first  = storage + i;
    buffer[i].second = storage + i;
  }

  dict =
    sp_phash_map_init(buffer, MAX, (sp_phash_map_hash_cb)sp_test_hash, NULL);

  for (i = 0; i < MAX; ++i) {
    uint32_t *value = sp_phash_map_lookup(dict, &i);
    printf("%u: %u\n", i, *value);
    assert(value);
    assert(*value == i);
  }

  sp_phash_map_free(&dict);
  return 0;
}
