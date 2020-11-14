#include "sp_queue.h"
#include <stdlib.h>
#include <assert.h>
#include "sp_buf.h"

//==============================
struct sp_queue {
  struct sp_buf *buffer;
};

//==============================
struct sp_queue *
sp_queue_init(void)
{
  struct sp_queue *result = calloc(1, sizeof(struct sp_queue));
  result->buffer = sp_buf_init(0);

  return result;
}

//==============================
sp_queue_T *
sp_queue_enqueue(struct sp_queue *self, sp_queue_T *in)
{
  sp_queue_T *result = NULL;

  assert(self);
  assert(in);

  size_t before = sp_buf_length(self->buffer);

  if (sp_buf_push_back(self->buffer, in)) {
    result = in;
    assert(sp_buf_length(self->buffer) == before + 1);
  }

  return result;
}

//==============================
bool
sp_queue_dequeue_impl(struct sp_queue *self, sp_queue_T **out)
{
  bool res;

  size_t before = sp_buf_length(self->buffer);

  res = sp_buf_pop_front(self->buffer, out);
  if (res) {
    assert(sp_buf_length(self->buffer) == before - 1);
  }

  return res;
}

//==============================
size_t
sp_queue_length(const struct sp_queue *self)
{
  return sp_buf_length(self->buffer);
}

bool
sp_queue_is_empty(const struct sp_queue *self)
{
  return sp_buf_is_empty(self->buffer);
}

//==============================
int
sp_queue_clear(struct sp_queue *self)
{
  sp_buf_clear(self->buffer);

  return 0;
}

//==============================
int
sp_queue_free(struct sp_queue **pself)
{
  assert(pself);
  struct sp_queue *self = *pself;

  if (self) {
    sp_buf_free(&self->buffer);
    free(self);
  }

  *pself = NULL;

  return 0;
}

//==============================
