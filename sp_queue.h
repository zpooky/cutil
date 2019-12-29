#ifndef _SP_QUEUE_H
#define _SP_QUEUE_H

#include <stdbool.h>
#include <stddef.h>

//==============================
struct sp_queue;

typedef void sp_queue_T;

//==============================
struct sp_queue *
sp_queue_init(void);

//==============================
sp_queue_T *
sp_queue_enqueue(struct sp_queue *, sp_queue_T *);

//==============================
bool
sp_queue_dequeue_impl(struct sp_queue *self, sp_queue_T **);

#define sp_queue_dequeue(self, out)                                            \
  sp_queue_dequeue_impl((self), (sp_queue_T **)(out))

//==============================
size_t
sp_queue_length(const struct sp_queue *);

bool
sp_queue_is_empty(const struct sp_queue *);

//==============================
int
sp_queue_clear(struct sp_queue *);

//==============================
int
sp_queue_free(struct sp_queue **);

//==============================
#endif
