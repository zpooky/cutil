#ifndef SP_HEAP_H
#define SP_HEAP_H

#include <stddef.h>
#include <stdbool.h>

#include <sp_callbacks.h>

//==============================
struct sp_heap;

typedef struct {
  size_t idx;
} sp_heap_T;

//==============================
struct sp_heap *sp_heap_init(sp_cb_cmp);

struct sp_heap *
sp_heap_init_copy(const struct sp_heap *o);

//==============================
sp_heap_T *
sp_heap_enqueue_impl(struct sp_heap *, sp_heap_T *);

#define sp_heap_enqueue(self, in)                                              \
  ((typeof(in))sp_heap_enqueue_impl((self), &(in)->base))

//==============================
bool
sp_heap_dequeue_impl(struct sp_heap *, sp_heap_T **, sp_heap_T *);

#define sp_heap_dequeue(self, out)                                             \
  sp_heap_dequeue_impl((self), (sp_heap_T **)(out), (typeof((*(out))->base) *)0)

//==============================
void *
sp_heap_head(struct sp_heap *);

//==============================
bool
sp_heap_remove_impl(struct sp_heap *, sp_heap_T *);

#define sp_heap_remove(self, in)                                               \
  sp_heap_remove_impl((self), (typeof((*(out))->base) *)in)

//==============================
void
sp_heap_update_key_impl(struct sp_heap *, sp_heap_T *subject);

#define sp_heap_update_key(self, subject)                                      \
  sp_heap_update_key_impl((self), &(subject)->base)

//==============================
size_t
sp_heap_length(const struct sp_heap *);

size_t
sp_heap_capacity(const struct sp_heap *);

//==============================
bool
sp_heap_is_empty(const struct sp_heap *);

//==============================
sp_heap_T **
sp_heap_array(struct sp_heap *);

//==============================
int
sp_heap_clear(struct sp_heap *);

//==============================
int
sp_heap_free(struct sp_heap **);

//==============================
#endif
