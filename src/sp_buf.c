#include "sp_buf.h"

#include <stdlib.h>
#include <assert.h>
#include <memory.h>

#include "sp_util.h"

//==============================
struct sp_buf {
  sp_buf_T **buffer;
  size_t read;
  size_t write;
  size_t capacity;
};

//==============================
struct sp_buf *
sp_buf_init(size_t cap)
{
  struct sp_buf *result;
  assert(cap % 2 == 0);

  if ((result = calloc(1, sizeof(*result)))) {
    if (cap > 0) {
      result->buffer = calloc(cap, sizeof(struct sp_buf_T *));
    }
    result->capacity = cap;
  }

  return result;
}

//==============================
size_t
sp_buf_length(const struct sp_buf *self)
{
  assert(self);
  assert(self->write >= self->read);

  return self->write - self->read;
}

//==============================
bool
sp_buf_is_empty(const struct sp_buf *self)
{
  assert(self);

  return self->read == self->write;
}

//==============================
void
sp_buf_clear(struct sp_buf *self)
{
  assert(self);

  self->read = 0;
  self->write = 0;
}

//==============================
static bool
sp_buf_is_full(struct sp_buf *self)
{
  assert(self);

  return sp_buf_length(self) == self->capacity;
}

static inline size_t
sp_buf_index(size_t in, size_t capacity)
{
  return in & (capacity - 1);
}

static void
sp_buf_copy(struct sp_buf *dest, struct sp_buf *src)
{
  /* sp_buf_T **raw_src = &src->buffer[sp_buf_index(src->read, src->capacity)]; */
  /* assert(dest->read == 0); */
  /* assert(dest->write == 0); */
  /*  */
  /* dest->read = sp_buf_index(src->read, src->capacity); */
  /* dest->write = sp_buf_index(src->write, src->capacity); */
  /* memcpy(dest->buffer, raw_src, sp_buf_length(src) * sizeof(sp_buf_T *)); */
  //XXX
  sp_buf_T *data = NULL;
  while (sp_buf_pop_front(src, &data)) {
    assert(data);
    sp_buf_push_back(dest, data);
  }
}

static void
sp_buf_swap(struct sp_buf *f, struct sp_buf *s)
{
  sp_util_swap_voidpp(&f->buffer, &s->buffer);
  sp_util_swap_size_t(&f->read, &s->read);
  sp_util_swap_size_t(&f->write, &s->write);
  sp_util_swap_size_t(&f->capacity, &s->capacity);
}

bool
sp_buf_push_back(struct sp_buf *self, sp_buf_T *in)
{
  size_t idx;

  assert(self);
  assert(in);

  if (sp_buf_is_full(self)) {
    struct sp_buf *tmp = sp_buf_init(sp_max(16, self->capacity * 2));
    if (!tmp) {
      return false;
    }

    sp_buf_copy(/*dest*/ tmp, self);
    sp_buf_swap(self, tmp);

    sp_buf_clear(tmp);
    sp_buf_free(&tmp);
  }

  idx = sp_buf_index(self->write++, self->capacity);
  self->buffer[idx] = in;

  return true;
}

//==============================
bool
sp_buf_peek_front(const struct sp_buf *self, sp_buf_T **out)
{
  size_t idx;

  assert(self);
  assert(out);

  if (sp_buf_is_empty(self)) {
    *out = NULL;
    return false;
  }

  idx = sp_buf_index(self->read, self->capacity);
  *out = self->buffer[idx];

  return true;
}

bool
sp_buf_peek_back(const struct sp_buf *self, sp_buf_T **out)
{
  size_t idx;

  assert(self);
  assert(out);

  if (sp_buf_is_empty(self)) {
    *out = NULL;
    return false;
  }

  idx = sp_buf_index(self->write - 1, self->capacity);
  *out = self->buffer[idx];

  return true;
}

//==============================
bool
sp_buf_pop_front(struct sp_buf *self, sp_buf_T **out)
{
  assert(self);
  assert(out);

  bool result = sp_buf_peek_front(self, out);
  if (result) {
    ++self->read;
  }

  return result;
}

bool
sp_buf_pop_back(struct sp_buf *self, sp_buf_T **out)
{
  assert(self);
  assert(out);

  bool result = sp_buf_peek_back(self, out);
  if (result) {
    --self->write;
  }

  return result;
}

//==============================
int
sp_buf_free(struct sp_buf **pself)
{
  assert(pself);

  struct sp_buf *self = *pself;

  if (self) {
    assert(sp_buf_is_empty(self));

    free(self->buffer);
    free(self);
    *pself = NULL;
  }

  return 0;
}

//==============================
