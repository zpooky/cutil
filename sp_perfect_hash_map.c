#include "sp_perfect_hash_map.h"

#include "sp_util.h"
#include "sp_vec.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

/* ======================================== */
struct sp_phash_map {
  uint32_t *indirect;
  struct sp_pair *values;
  size_t length;

  sp_phash_map_hash_cb hash;
  void *closure;
};

/* ======================================== */
static int
sp_phash_map_bucket_cmp(struct sp_vec * /*sp_pair*/ f,
                        struct sp_vec * /*sp_pair*/ s)
{
  return 0;
}

static void
ix(struct sp_phash_map *self, struct sp_pair *entries, size_t len)
{
  uint32_t d = 0;
  size_t i;
  struct sp_vec * /*sp_pair*/ bucket[len];
  memset(&bucket, 0, sizeof(bucket));

  for (i = 0; i < len; ++i) {
    uint32_t hash;
    size_t idx;

    hash = self->hash(d, entries[i].first, self->closure);
    idx  = hash % len;

    if (!bucket[idx]) {
      bucket[idx] = sp_vec_init();
    }
    sp_vec_append(bucket[idx], entries + i);
  }

  sp_util_sort(&bucket, len, (sp_util_sort_cmp_cb)sp_phash_map_bucket_cmp);
}

/* http://cmph.sourceforge.net/papers/esa09.pdf */
/* http://stevehanov.ca/blog/?id=119 */
struct sp_phash_map *
sp_phash_map_init(struct sp_pair *entries,
                  size_t len,
                  sp_phash_map_hash_cb hash,
                  void *closure)
{
  struct sp_phash_map *result;

  if ((result = calloc(1, sizeof(*result)))) {
    result->indirect = calloc(len, sizeof(*result->indirect));
    result->values   = calloc(len, sizeof(*result->values));
    result->length   = len;

    result->hash    = hash;
    result->closure = closure;

    ix(result, entries, len);
  }

  return result;
}

void
sp_phash_map_free(struct sp_phash_map **pself)
{
  assert(pself);
  if (*pself) {
    //TODO
  }
}

/* ======================================== */
sp_phash_map_Value *
sp_phash_map_lookup(struct sp_phash_map *, sp_phash_map_Key *);

/* ======================================== */
