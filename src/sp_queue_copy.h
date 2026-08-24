#ifndef _SP_QUEUE_COPY_H
#define _SP_QUEUE_COPY_H

#include <stdbool.h>
#include <stddef.h>
#include <sp_callbacks.h>

//==============================
struct sp_queue_copy;

typedef void sp_queue_copy_T;

//==============================
struct sp_queue_copy *
sp_queue_copy_new(size_t capacity, size_t element_align, size_t element_sz, sp_cb_copy);

int
sp_queue_copy_free(struct sp_queue_copy **);

//==============================
sp_queue_copy_T *
sp_queue_copy_enqueue(struct sp_queue_copy *, sp_queue_copy_T *);

//==============================
bool
sp_queue_copy_dequeue_impl(struct sp_queue_copy *self, sp_queue_copy_T *);

#define sp_queue_copy_dequeue(self, out)                                       \
  sp_queue_copy_dequeue_impl((self), (sp_queue_copy_T *)(out))

//==============================
size_t
sp_queue_copy_length(const struct sp_queue_copy *);

bool
sp_queue_copy_is_empty(const struct sp_queue_copy *);

//==============================
int
sp_queue_copy_clear(struct sp_queue_copy *);

//==============================

#endif
