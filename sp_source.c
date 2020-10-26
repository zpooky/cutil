#include "sp_source.h"

#include <assert.h>
#include <stdlib.h>
#include <errno.h>

#include <sp_util.h>

#include "sp_cbb.h"

//==============================
struct sp_source {
  sp_source_read_cb read_cb;
  struct sp_cbb *buffer;
  void *arg;
};

//==============================
struct sp_source *
sp_source_init(sp_source_read_cb r, size_t cap, void *arg)
{
  struct sp_source *result;
  assert(r);

  if ((result = calloc(1, sizeof(*result)))) {
    result->read_cb = r;
    result->buffer  = sp_cbb_init(cap);
    result->arg     = arg;
  }

  return result;
}

//==============================
bool
sp_source_read(struct sp_source *self, void *out, size_t out_len)
{
  assert(self);
  if (out_len > sp_cbb_capacity(self->buffer)) {
    assert(false);
    return false;
  }

  if (sp_cbb_remaining_read(self->buffer) < out_len) {
    if (sp_cbb_is_readonly(self->buffer)) {
      assert(false);
      return false;
    }

    self->read_cb(self->buffer, self->arg);
  }

  return sp_cbb_read(self->buffer, out, out_len);
}

//==============================
size_t
sp_source_pop_front(struct sp_source *, void *, size_t);

//==============================
size_t
sp_source_capacity(const struct sp_source *self)
{
  assert(self);
  return sp_cbb_capacity(self->buffer);
}

//==============================
int
sp_source_mark(struct sp_source *self, sp_source_mark_t *out)
{
  sp_cbb_mark_t mark = {0};
  assert(self);
  sp_cbb_read_mark(self->buffer, &mark);
  out->before   = mark.before;
  out->rollback = mark.rollback;

  return 0;
}

int
sp_source_unmark(struct sp_source *self, const sp_source_mark_t *in)
{
  sp_cbb_mark_t mark = {
    .before   = in->before,
    .rollback = in->rollback,
  };
  assert(self);
  return sp_cbb_read_unmark(self->buffer, &mark);
}

//==============================
size_t
sp_source_consume_bytes(struct sp_source *self, size_t bytes)
{
  while (bytes) {
    size_t l = sp_util_min(sp_cbb_remaining_read(self->buffer), bytes);
    if (!sp_cbb_consume_bytes(self->buffer, bytes)) {
      assert(false);
      break;
    }

    bytes -= l;
    if (bytes > 0) {
      if (sp_cbb_is_readonly(self->buffer)) {
        break;
      }

      self->read_cb(self->buffer, self->arg);
      if (sp_cbb_remaining_read(self->buffer) == 0) {
        break;
      }
    }
  }

  return bytes;
}

//==============================
struct sp_cbb *
sp_source_reaonly_view(struct sp_source *self, size_t length)
{
  assert(self);

  if (length > sp_cbb_capacity(self->buffer)) {
    return NULL;
  }

  if (sp_cbb_remaining_read(self->buffer) < length) {
    if (sp_cbb_is_readonly(self->buffer)) {
      assert(false);
      return NULL;
    }

    self->read_cb(self->buffer, self->arg);
  }

  return sp_cbb_readonly_view(self->buffer, length);
}

struct sp_cbb *
sp_source_consume_reaonly_view(struct sp_source *self, size_t length)
{
  //XXX copy of ^ {
  assert(self);

  if (length > sp_cbb_capacity(self->buffer)) {
    return NULL;
  }

  if (sp_cbb_remaining_read(self->buffer) < length) {
    if (sp_cbb_is_readonly(self->buffer)) {
      assert(false);
      return NULL;
    }

    self->read_cb(self->buffer, self->arg);
  }
  //}

  return sp_cbb_consume_readonly_view(self->buffer, length);
}

//==============================
