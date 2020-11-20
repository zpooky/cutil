#include "sp_bloom_filter.h"

#include <sp_bitset.h>

#include <stdlib.h>
#include <assert.h>

#include <sp_fnv.h>
#include <sp_djb.h>

//==============================
struct sp_bloom_filter {
  struct sp_bitset *set;
};

//==============================
struct sp_bloom_filter *
sp_bloom_filter_init(size_t bits)
{
  struct sp_bloom_filter *result;

  if ((result = calloc(1, sizeof(*result)))) {
    result->set = sp_bitset_init(bits);
  }

  return result;
}

void
sp_bloom_filter_free(struct sp_bloom_filter **pself)
{
  assert(pself);
  if (*pself) {
    struct sp_bloom_filter *self = *pself;

    sp_bitset_free(&self->set);
    free(self);
    *pself = NULL;
  }
}

//==============================
bool
sp_bloom_filter_test(const struct sp_bloom_filter *self,
                     const void *raw,
                     size_t l)
{
  size_t idx;

  idx = sp_fnv1a_hash(raw, l);
  if (!sp_bitset_test(self->set, idx)) {
    return false;
  }

  idx = sp_djb2a_hash(raw, l);
  return sp_bitset_test(self->set, idx);
}

//==============================
void
sp_bloom_filter_set(struct sp_bloom_filter *self, const void *raw, size_t l)
{
  size_t idx;

  idx = sp_fnv1a_hash(raw, l);
  sp_bitset_set(self->set, idx, true);

  idx = sp_djb2a_hash(raw, l);
  sp_bitset_set(self->set, idx, true);
}

//==============================
void
sp_bloom_filter_clear(struct sp_bloom_filter *self)
{
  sp_bitset_set_all(self->set, 0);
}

//==============================
