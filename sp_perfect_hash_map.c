#include "sp_perfect_hash_map.h"

#include "sp_util.h"
#include "sp_vec.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
sp_phash_map_vec_max_cmp(struct sp_vec * /*sp_pair*/ f,
                         struct sp_vec * /*sp_pair*/ s)
{
  size_t f_len = f ? sp_vec_length(f) : 0;
  size_t s_len = s ? sp_vec_length(s) : 0;
  return sp_util_size_t_cmp(s_len, f_len);
}

static void
ix(struct sp_phash_map *self, struct sp_pair *entries, size_t len)
{
  size_t i;
  uint32_t ind_hash;
  size_t ind_idx;

  struct sp_vec * /*sp_pair*/ cluster[len];
  memset(&cluster, 0, sizeof(cluster));

  for (i = 0; i < len; ++i) {
    ind_hash = self->hash(0, entries[i].first, self->closure);
    ind_idx  = ind_hash % len;

    if (!cluster[ind_idx]) {
      cluster[ind_idx] = sp_vec_init();
    }
    sp_vec_append(cluster[ind_idx], &entries[i]);
  }

  sp_util_sort((void **)&cluster, len,
               (sp_util_sort_cmp_cb)sp_phash_map_vec_max_cmp);

  for (i = 0; i < len; ++i) {
    size_t a;

    size_t cluster_len = cluster[i] ? sp_vec_length(cluster[i]) : 0;
    uint32_t seed      = 0;
    uint32_t item      = 0;

    struct sp_pair *tmp_values[len];
    memset(tmp_values, 0, sizeof(tmp_values));

    if (cluster_len == 0) {
      break;
    }
    printf("- %zu[len:%zu,i:%zu]\n", cluster_len, self->length, i);

    while (item < cluster_len) {
      size_t slot;

      slot =
        self->hash(seed, sp_vec_get(cluster[i], item), self->closure) % len;
      if (self->values[slot].first || tmp_values[slot]) {
        ++seed;
        item = 0;
        memset(tmp_values, 0, sizeof(tmp_values));
      } else {
        tmp_values[slot] = sp_vec_get(cluster[i], item);
        ++item;
      }

    } //while

    for (a = 0; a < len; ++a) {
      if (tmp_values[a]) {
        assertx(!(self->values[a].first || self->values[a].second));
        sp_pair_set(&self->values[a], tmp_values[a]);
      }
    } //for

    ind_hash = self->hash(0, sp_vec_get(cluster[i], 0), self->closure);
    ind_idx  = ind_hash % len;
    self->indirect[ind_idx] = seed;

  } //for
  printf("--\n");

  for (; i < len; ++i) {
    size_t cluster_len = cluster[i] ? sp_vec_length(cluster[i]) : 0;
    printf("- %zu[len:%zu,i:%zu]\n", cluster_len, self->length, i);

    assert(!cluster[i]);
  }

  /* for (; i < len; ++i) { */
  /*   size_t cluster_len = cluster[i] ? sp_vec_length(cluster[i]) : 0; */
  /*   assert(cluster_len <= 1); */
  /*  */
  /*   if (cluster_len == 0) { */
  /*     break; */
  /*   } */
  /*  */
  /*   ind_hash = self->hash(0, , self->closure); */
  /*   ind_idx  = ind_hash % len; */
  /*  */
  /*   //TODO process all with length=1 */
  /* } //for */
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

  assert(entries);
  assert(len > 0);
  assert(hash);

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
    struct sp_phash_map *self = *pself;
    free(self->indirect);
    free(self->values);
    free(self);

    *pself = NULL;
  }
}

/* ======================================== */
sp_phash_map_Value *
sp_phash_map_lookup(struct sp_phash_map *self, sp_phash_map_Key *key)
{
  uint32_t hash;
  size_t idx;
  uint32_t seed;

  assert(self);
  assert(key);

  hash = self->hash(0, key, self->closure);
  idx  = hash % self->length;

  seed = self->indirect[idx];
  hash = self->hash(seed, key, self->closure);
  idx  = hash % self->length;

  return self->values[idx].second;
}

/* ======================================== */
