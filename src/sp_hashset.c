#include "sp_hashset.h"

#include <stdlib.h>
#include <assert.h>
#include <memory.h>
#include <stdalign.h>

#include "sp_util.h"

/* #include "sp_vec_copy.h" */
/* #include "sp_vec_copy_internal.h" */

//==============================
typedef uint8_t PSL_t;
/* #define PSL_EMPTY ((PSL_t) ~((PSL_t)0)) */
#define PSL_EMPTY 0
#define PSL_INITIAL 1

struct sp_hashset {
  /* struct sp_vec_copy #<{(|T|)}># bucket; */
  /* struct sp_vec_copy #<{(|int32|)}># psl; */

  size_t sz;
  size_t align;

  uint8_t *bucket;
  PSL_t *psl;
  size_t length;
  size_t capacity;

  sp_hashset_hash_cb hash;
  sp_hashset_copy_cb copy;
  sp_hashset_eq_cb eq;
};

//==============================
static void
sp_hashset_memcopy(sp_hashset_T *dest, const sp_hashset_T *src, size_t sz)
{
  memcpy(dest, src, sz);
}

static void
sp_hashset_init_int(struct sp_hashset *self,
                    size_t capacity,
                    size_t align,
                    size_t sz,
                    sp_hashset_hash_cb hash,
                    sp_hashset_copy_cb copy,
                    sp_hashset_eq_cb eq)
{
  if (capacity) {
    assert(capacity % 8 == 0);
    self->bucket = aligned_alloc(align, capacity * sz);
    self->psl    = calloc(capacity, sizeof(*self->psl));
  }

  self->sz    = sz;
  self->align = align;

  self->length   = 0;
  self->capacity = capacity;

  self->hash = hash;
  self->copy = copy;
  self->eq   = eq;
}

static void
sp_hashset_swap_self(struct sp_hashset *f, struct sp_hashset *s)
{
  sp_util_swap_size_t(&f->sz, &s->sz);
  sp_util_swap_size_t(&f->align, &s->align);

  sp_util_swap_voidp(&f->bucket, &s->bucket);
  sp_util_swap_voidp(&f->psl, &s->psl);
  sp_util_swap_size_t(&f->length, &s->length);
  sp_util_swap_size_t(&f->capacity, &s->capacity);

  sp_util_swap_voidp(&f->hash, &s->hash);
  sp_util_swap_voidp(&f->copy, &s->copy);
  sp_util_swap_voidp(&f->eq, &s->eq);
}

struct sp_hashset *
sp_hashset_init(size_t align,
                size_t sz,
                sp_hashset_hash_cb hash,
                sp_hashset_copy_cb copy,
                sp_hashset_eq_cb eq)
{
  struct sp_hashset *self;

  if ((self = calloc(1, sizeof(*self)))) {
    sp_hashset_init_int(self, 0, align, sz, hash, copy, eq);
  }

  return self;
}

static void
sp_hashset_free_int(struct sp_hashset *self)
{
  if (self->bucket) {
    free(self->bucket);
    self->bucket = NULL;
  }

  if (self->psl) {
    free(self->psl);
    self->psl = NULL;
  }
}

int
sp_hashset_free(struct sp_hashset **pself)
{
  assert(pself);

  if (*pself) {
    struct sp_hashset *self = *pself;
    sp_hashset_free_int(self);

    free(self);
    *pself = NULL;
  }

  return 0;
}

//==============================
static sp_hashset_T *
sp_hashset_get(struct sp_hashset *self, size_t idx)
{
  return self->bucket + (idx * self->sz);
}

//==============================
static bool
sp_hashset_is_full(struct sp_hashset *self)
{
  //TODO
  return true;
}

static void
sp_hashset_rehash(struct sp_hashset *self)
{
  struct sp_hashset tmp = {0};
  size_t ncap           = sp_util_min(16, self->capacity * 2) + 1;
  size_t i;

  sp_hashset_init_int(&tmp, ncap, self->align, self->sz, self->hash, self->copy,
                      self->eq);

  for (i = 0; i < self->capacity && self->length < tmp.length; ++i) {
    if (self->psl[i] != PSL_EMPTY) {
      sp_hashset_T *res;
      res = sp_hashset_insert(&tmp, sp_hashset_get(self, i));
      assert(res);
    }
  }

  sp_hashset_swap_self(self, &tmp);
  sp_hashset_free_int(&tmp);
}

static void
sp_hashset_swap(struct sp_hashset *self, sp_hashset_T *f, sp_hashset_T *s)
{
  /* the last index in self->bucket is used for tmp swap */
  sp_hashset_T *tmp = sp_hashset_get(self, self->capacity);
  self->copy(tmp, f, self->sz);
  self->copy(s, tmp, self->sz);
}

sp_hashset_T *
sp_hashset_insert(struct sp_hashset *self, sp_hashset_T *in)
{
  sp_hashset_T *result = NULL;
  uint32_t it, idx;
  PSL_t PSL = PSL_INITIAL;

  assert(self);
  assert(in);

  if (sp_hashset_is_full(self)) {
    sp_hashset_rehash(self);
  }

  it = idx = self->hash(in) & self->capacity;
  do {
    sp_hashset_T *dest = sp_hashset_get(self, it);

    if (self->psl[it] == PSL_EMPTY) {
      result = result ? result : dest;
      self->copy(dest, in, self->sz);
      self->psl[it] = PSL;
      return result;
    }

    if (PSL > self->psl[it]) {
      result = result ? result : dest;
      sp_hashset_swap(self, in, dest);
      sp_util_swap_uint8_t(&self->psl[it], &PSL);
    }

    /* TODO how to ensure that PSL not becomes PSL_EMPTY? */
    PSL++;

    it = (it + 1) & self->capacity;
  } while (it != idx);

  return result;
}

//==============================
sp_hashset_T *
sp_hashset_lookup(struct sp_hashset *self, sp_hashset_T *needle)
{
  uint32_t it, idx;
  PSL_t PSL = PSL_INITIAL;

  if (sp_hashset_is_empty(self)) {
    return NULL;
  }
  it = idx = self->hash(needle) & self->capacity;

  do {
    sp_hashset_T *bucket;
    PSL_t psl_data = self->psl[it];

    if (psl_data == PSL_EMPTY)
      return NULL;

    bucket = sp_hashset_get(self, it);
    if (self->eq(needle, bucket, self->sz)) {
      return bucket;
    }

    if (psl_data > PSL) {
      // If $needle had been in the table, it would have been located before this
      // bucket[i], since when inserting during probing we always swap with a
      // bucket[i] that has a greater PSL, beacuse of that we can be sure that
      // when encountering a bucket with greater PSL we can be sure that the
      // $needle is present.
      return NULL;
    }

    ++PSL;
    it = (it + 1) & self->capacity;
  } while (it != idx);

  return NULL;
}

//==============================
bool
sp_hashset_remove(struct sp_hashset *self, sp_hashset_T *);

//==============================
bool
sp_hashset_is_empty(const struct sp_hashset *self)
{
  return self->length == 0;
}

//==============================
