#include "sp_source.h"

#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#include <sp_util.h>

#include "sp_cbb.h"

//==============================
struct sp_source {
  sp_source_read_cb read_cb;
  struct sp_cbb *buffer;
  void *arg;
  int error;
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
int
sp_source_free(struct sp_source **pself)
{
  assert(pself);
  if (*pself) {
    struct sp_source *self = *pself;
    sp_cbb_clear(self->buffer);
    sp_cbb_free(&self->buffer);
    free(self);
    *pself = NULL;
  }

  return 0;
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

    self->error = self->read_cb(self->buffer, self->arg);
  }

  return sp_cbb_read(self->buffer, out, out_len);
}

//==============================
size_t
sp_source_pop_front(struct sp_source *self, void *out, size_t out_len)
{
  size_t result;
  assert(self);

  result = sp_source_peek_front(self, out, out_len);
  if (!sp_source_consume_bytes(self, result)) {
    //bug
    assert(false);
    result = 0;
  }

  return result;
}

//==============================
size_t
sp_source_peek_front(struct sp_source *self, void *out, size_t out_len)
{
  assert(self);
  if (sp_cbb_remaining_read(self->buffer) < out_len) {
    if (!sp_cbb_is_full(self->buffer)) {
      if (!sp_cbb_is_readonly(self->buffer)) {

        self->error = self->read_cb(self->buffer, self->arg);
      }
    }
  }

  return sp_cbb_peek_front(self->buffer, out, out_len);
}

//==============================
int
sp_source_error(const struct sp_source *self){
  return self->error;
}

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
  out->l_commit_hooks = 0;

  sp_cbb_read_mark(self->buffer, &mark);
  out->before   = mark.before;
  out->rollback = mark.rollback;

  return 0;
}

int
sp_source_unmark(struct sp_source *self, const sp_source_mark_t *in)
{
  size_t i;
  sp_cbb_mark_t mark = {
    .before         = in->before,
    .rollback       = in->rollback,
    .l_commit_hooks = in->l_commit_hooks,
  };
  assert(self);
  for (i = 0; i < mark.l_commit_hooks; ++i) {
    mark.commit_hooks[i]        = in->commit_hooks[i];
    mark.commit_hooks_closure[i] = in->commit_closure[i];
  }

  return sp_cbb_read_unmark(self->buffer, &mark);
}

bool
sp_source_is_marked(const struct sp_source *self)
{
  return sp_cbb_is_read_mark(self->buffer);
}

//==============================
bool
sp_source_consume_bytes(struct sp_source *self, size_t bytes)
{
  if (bytes > sp_source_capacity(self)) {
    //bug
    assert(false);
    return false;
  }

  if (sp_cbb_remaining_read(self->buffer) < bytes) {
    if (sp_cbb_is_readonly(self->buffer)) {
      return false;
    }

    self->error =self->read_cb(self->buffer, self->arg);

    if (sp_cbb_remaining_read(self->buffer) < bytes) {
      return false;
    }
  }

  if (!sp_cbb_consume_bytes(self->buffer, bytes)) {
    //bug
    assert(false);
  }

  return true;
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

    self->error =self->read_cb(self->buffer, self->arg);
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

    self->error =self->read_cb(self->buffer, self->arg);
  }
  //}

  return sp_cbb_consume_readonly_view(self->buffer, length);
}

//==============================
void
sp_source_dump_hex(struct sp_source *self)
{
  struct sp_cbb_Arr arr[2] = {0};
  size_t alen              = 0;
  size_t p;

  assert(self);

  if (sp_cbb_remaining_read(self->buffer) == 0) {
    if (!sp_cbb_is_readonly(self->buffer)) {

      self->error =self->read_cb(self->buffer, self->arg);
    }
  }

  /* printf("%s: read:%zu\n", __func__, sp_cbb_remaining_read(self->buffer)); */

  alen = sp_cbb_read_buffer(self->buffer, arr);
  for (p = 0; p < alen; ++p) {
    sp_util_to_hex(NULL, arr[p].base, arr[p].len);
  }
}

//==============================
void
sp_source_eager_fill(struct sp_source *self)
{
  if (!sp_cbb_is_full(self->buffer)) {
    if (!sp_cbb_is_readonly(self->buffer)) {
      self->error =self->read_cb(self->buffer, self->arg);
    }
  }
}

//==============================
void
sp_source_get_internal_state(struct sp_source *self,
                             sp_source_read_cb *r,
                             struct sp_cbb **buffer,
                             void **arg)
{
  assert(self);
  if (r) {
    *r = self->read_cb;
  }
  if (buffer) {
    *buffer = self->buffer;
  }
  if (arg) {
    *arg = self->arg;
  }
}

void
sp_source_set_internal_state(struct sp_source *self,
                             sp_source_read_cb r,
                             struct sp_cbb *buffer,
                             void *arg)
{
  assert(self);
  if (r) {
    self->read_cb = r;
  }
  if (buffer) {
    self->buffer = buffer;
  }
  if (arg) {
    self->arg = arg;
  }
}

//==============================
bool
sp_source_ensure_at_least_readable(struct sp_source *self, size_t len)
{
  assert(self);
  if (len > sp_source_capacity(self)) {
    assert(false);
    return false;
  }

  if (sp_cbb_remaining_read(self->buffer) >= len) {
    return true;
  }

  if (!sp_cbb_is_readonly(self->buffer)) {
    self->error =self->read_cb(self->buffer, self->arg);
    if (sp_cbb_remaining_read(self->buffer) >= len) {
      return true;
    }
  }

  return false;
}

//==============================
size_t sp_source_debug_in_cbb(const struct sp_source *self){
  return sp_cbb_remaining_read(self->buffer);
}

//==============================
