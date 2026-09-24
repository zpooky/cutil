#include "sp_hashset.h"

#include <stdlib.h>
#include <assert.h>
#include <memory.h>
#include <stdalign.h>
#include <limits.h>
#include <stdio.h>

#include "sp_util.h"

/* #include "sp_vec_copy.h" */
/* #include "sp_vec_copy_internal.h" */

//==============================
#if 1
typedef uint8_t PSL_t;
#define PSL_MAX UCHAR_MAX
#define swap_GENERIC sp_util_swap_uint8_t
#else
typedef uint16_t PSL_t;
#define PSL_MAX UINT16_MAX
#define swap_GENERIC sp_util_swap_uint16_t
#endif

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

  sp_cb_hash hash;
  sp_cb_move move;
  sp_cb_eq eq;

  sp_hashset_clear_cb clear;
  void *clear_closure;
};

#define INDEX_(hash, capacity) ((hash) & ((capacity) - 1))
/* #define INDEX_(hash, capacity) ((hash) % (capacity)) */

//==============================
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

static bool
sp_hashset_init_int(struct sp_hashset *self,
                    size_t capacity,
                    size_t align,
                    size_t sz,
                    sp_cb_hash hash,
                    sp_cb_move move,
                    sp_cb_eq eq)
{
  self->bucket = NULL;
  self->psl    = NULL;
  if (capacity) {
    assert(sp_util_is_power_of_two(capacity));
    size_t alloc_sz = (capacity + 2) * sz;
    self->bucket    = aligned_alloc(align, alloc_sz);
    memset(self->bucket, 0, alloc_sz);
    self->psl = calloc(capacity, sizeof(*self->psl));
    /* printf("sizeof(self->psl):%zuMiB, sizeof(self->bucket):%zuMiB\n", */
    /*        (capacity * sizeof(*self->psl)) / 1024 / 1024, */
    /*        alloc_sz / 1024 / 1024); */

    if (!(self->bucket && self->psl)) {
      sp_hashset_free_int(self);
      return false;
    }
  }

  self->sz    = sz;
  self->align = align;

  self->length   = 0;
  self->capacity = capacity;

  self->hash = hash;
  self->move = move;
  self->eq   = eq;

  return true;
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
  sp_util_swap_voidp(&f->move, &s->move);
  sp_util_swap_voidp(&f->eq, &s->eq);
}

struct sp_hashset *
sp_hashset_new(size_t align,
               size_t sz,
               sp_cb_hash hash,
               sp_cb_move move,
               sp_cb_eq eq)
{
  struct sp_hashset *self;

  if ((self = calloc(1, sizeof(*self)))) {
    sp_hashset_init_int(self, 0, align, sz, hash, move, eq);
  }

  return self;
}

int
sp_hashset_free(struct sp_hashset **pself)
{
  assert(pself);

  if (*pself) {
    struct sp_hashset *self = *pself;

    sp_hashset_clear(self);
    sp_hashset_free_int(self);

    free(self);
    *pself = NULL;
  }

  return 0;
}

//==============================
static sp_T *
sp_hashset_get(struct sp_hashset *self, size_t idx)
{
  return self->bucket + (idx * self->sz);
}

static size_t
sp_hashset_full_length(size_t length, const struct sp_hashset *self)
{
  return length + sp_max(self->capacity * 0.10, 1);
}

static bool
sp_hashset_is_full(struct sp_hashset *self)
{
  /* size_t c = sp_min(self->capacity / PSL_MAX, 1); */
  size_t length = sp_hashset_full_length(self->length, self);
  if (length >= self->capacity) {
    printf("length:%zu >= self->capacity:%zu (self->length:%zu)\n", length,
           self->capacity, self->length);
    return true;
  }
  return false;
}

static bool
sp_hashset_rehash(struct sp_hashset *self, size_t in_cap)
{
  struct sp_hashset tmp = {0};
  size_t new_cap;
  size_t i;
  bool res;

  new_cap = sp_max(sp_max(8, self->capacity * 2), in_cap);
  res = sp_hashset_init_int(&tmp, new_cap, self->align, self->sz, self->hash,
                            self->move, self->eq);
  if (!res) {
    assert(false);
    return false;
  }

  for (i = 0; i < self->capacity && tmp.length < self->length; ++i) {
    if (self->psl[i] != PSL_EMPTY) {
      sp_T *res;
      res = sp_hashset_insert_move(&tmp, sp_hashset_get(self, i));
      assert(res);
    }
  }

  sp_hashset_swap_self(self, &tmp);
  sp_hashset_free_int(&tmp);

  return true;
}

static size_t
sp_next_power_of_two(size_t value)
{
  size_t result = 8;

  while (result < value) {
    assert(result <= SIZE_MAX / 2);
    result *= 2;
  }

  return result;
}

bool
sp_hashset_ensure_capacity(struct sp_hashset *self, size_t capacity)
{
  size_t req_length = sp_hashset_full_length(capacity, self);
  if (self->capacity < req_length) {
    if (!sp_util_is_power_of_two(req_length)) {
      req_length = sp_next_power_of_two(req_length);
    }
    return sp_hashset_rehash(self, req_length);
  }
  return true;
}

//==============================
void
sp_hashset_set_clear_cb(struct sp_hashset *self,
                        sp_hashset_clear_cb cb,
                        void *closure)
{
  self->clear         = cb;
  self->clear_closure = closure;
}

void
sp_hashset_clear(struct sp_hashset *self)
{
  if (self->clear) {
    size_t i;
    for (i = 0; i < self->capacity && self->length > 0; ++i) {
      if (self->psl[i] != PSL_EMPTY) {
        self->clear(sp_hashset_get(self, i), self->sz, self->clear_closure);
        self->psl[i] = PSL_EMPTY;
        --self->length;
      }
    } //for
    assertx(self->length == 0);
  }
  self->length = 0;
}

//==============================
bool
__sp_hashset_validate(struct sp_hashset *self)
{
  size_t i;
  for (i = 0; i < self->capacity; ++i) {
    if (self->psl[i] != PSL_EMPTY) {
      size_t idx;
      sp_T *entry;

      entry = sp_hashset_get(self, i);
      idx   = INDEX_(self->hash(entry), self->capacity);

      if (idx == i) {
        assertx(self->psl[i] == PSL_INITIAL);
      } else {
        size_t a   = i;
        size_t cnt = self->psl[i] - PSL_INITIAL;
        while (cnt) {
          a = a == 0 ? self->capacity - 1 : a - 1;
          assertx(self->psl[a] != PSL_EMPTY);
          {

            /* assertx(self->psl[a] <= self->psl[i]); */
          }
          --cnt;
        }
        assert(INDEX_(a + (self->psl[i] - PSL_INITIAL), self->capacity) == i);
      }
    }
  } //for
  return true;
}

//==============================

static void
sp_hashset_swap(struct sp_hashset *self, sp_T *f, sp_T *s)
{
  /* the last index in self->bucket is used for tmp swap */
  sp_T *scrach = sp_hashset_get(self, self->capacity + 1);
  self->move(/*dest*/ scrach, f, self->sz);
  self->move /*dest*/ (f, s, self->sz);
  self->move /*dest*/ (s, scrach, self->sz);
}

sp_T *
sp_hashset_insert_move(struct sp_hashset *self, sp_T *in)
{
  sp_T *result = NULL;
  size_t it, start;
  PSL_t PSL = PSL_INITIAL;
  sp_T *scrach;

  assert(self);
  assert(in);

  if (sp_hashset_is_full(self)) {
    if (!sp_hashset_rehash(self, 0)) {
      assert(false);
      return NULL;
    }
  }

  scrach = sp_hashset_get(self, self->capacity);
  self->move(/*dest*/ scrach, in, self->sz);
  it = start = INDEX_(self->hash(scrach), self->capacity);
  do {
    sp_T *dest = sp_hashset_get(self, it);

    if (self->psl[it] == PSL_EMPTY) {
      result = result ? result : dest;
      self->move(/*dest*/ dest, scrach, self->sz);
      self->psl[it] = PSL;
      goto Lout;
    }

    if (PSL > self->psl[it]) {
      result = result ? result : dest;
      sp_hashset_swap(self, scrach, dest);
      swap_GENERIC(&self->psl[it], &PSL);
    }

    /* TODO how to ensure that PSL not becomes PSL_EMPTY? */
    PSL++;
    assert(PSL < PSL_MAX);

    it = INDEX_((it + 1), self->capacity);
  } while (it != start);

Lout:
  if (result) {
    self->length++;
    /* assert(sp_hashset_validate(self)); */
  }
  return result;
}

//==============================
static bool
sp_hashset_do_lookup(struct sp_hashset *self,
                     sp_T *needle,
                     size_t *it,
                     size_t *start)
{
  PSL_t PSL = PSL_INITIAL;

  if (sp_hashset_is_empty(self))
    return false;

  *it = *start = INDEX_(self->hash(needle), self->capacity);

  do {
    sp_T *bucket;

    if (self->psl[*it] == PSL_EMPTY)
      return false;

    bucket = sp_hashset_get(self, *it);
    if (self->eq(needle, bucket, self->sz)) {
      return true;
    }

    if (PSL > self->psl[*it]) {
      // If $needle had been in the table, it would have been located before this
      // bucket[i], since when inserting during probing we always swap with a
      // bucket[i] that has a greater PSL, beacuse of that we can be sure that
      // when encountering a bucket with greater PSL we can be sure that the
      // $needle is present.
      return false;
    }

    ++PSL;
    *it = INDEX_((*it + 1), self->capacity);
  } while (*it != *start);

  return false;
}

sp_T *
sp_hashset_lookup(struct sp_hashset *self, sp_T *needle)
{
  size_t it, start;

  if (sp_hashset_do_lookup(self, needle, &it, &start)) {
    return sp_hashset_get(self, it);
  }

  return NULL;
}

//==============================
sp_T *
sp_hashset_upsert_move(struct sp_hashset *self, sp_T *in)
{
  size_t it, start;
  //XXX uptimize?
  if (sp_hashset_do_lookup(self, in, &it, &start)) {
    sp_T *result = sp_hashset_get(self, it);
    self->move(/*dest*/ result, in, self->sz);
    return result;
  }
  return sp_hashset_insert_move(self, in);
}

//==============================
bool
sp_hashset_remove(struct sp_hashset *self, sp_T *needle)
{
  size_t it, start;
  bool result;

  if ((result = sp_hashset_do_lookup(self, needle, &it, &start))) {
    size_t priv;
    --self->length;

    assert(self->eq(sp_hashset_get(self, it), needle, self->sz));

    self->move(/*dest*/ needle, sp_hashset_get(self, it), self->sz);
    self->psl[it] = PSL_EMPTY;

    priv = start = it;
    it           = INDEX_((it + 1), self->capacity);

    while (it != start) {
      if (self->psl[it] > PSL_INITIAL) {
        void *p = sp_hashset_get(self, priv);
        void *s = sp_hashset_get(self, it);
        sp_hashset_swap(self, p, s);
        swap_GENERIC(&self->psl[priv], &self->psl[it]);
        self->psl[priv]--;
      } else {
        break;
      }

      priv = it;
      it   = INDEX_((it + 1), self->capacity);
    } //while
    /* assert(sp_hashset_validate(self)); */
  }

  return result;
}

//==============================
bool
sp_hashset_is_empty(const struct sp_hashset *self)
{
  return self->length == 0;
}

size_t
sp_hashset_length(const struct sp_hashset *self)
{
  return self->length;
}

//==============================
void
sp_hashset_dump(struct sp_hashset *self)
{
  size_t i, a;
  for (i = 0, a = 0; i < self->capacity && a < self->length; ++i) {
    if (self->psl[i] != PSL_EMPTY) {
      sp_T *entry = sp_hashset_get(self, i);
      uint32_t h  = self->hash(entry);
      fprintf(stderr, "%zu[h:%u, [%p] idx[%zu]], ", i, h, entry,
              INDEX_(h, self->capacity));
      ++a;
    }
  }
  fprintf(stderr, "\n");
  fflush(stderr);
}

//==============================
int
sp_hashset_for_each(const struct sp_hashset *self,
                    sp_hashset_for_each_cb cb,
                    void *closure)
{
  int res = 0;
  size_t i, a;
  for (i = 0, a = 0; i < self->capacity && a < self->length && res == 0; ++i) {
    if (self->psl[i] != PSL_EMPTY) {
      const sp_T *entry = sp_hashset_get((struct sp_hashset *)self, i);

      res = cb(entry, closure, self->sz);
      ++a;
    }
  }

  return res;
}
//==============================
