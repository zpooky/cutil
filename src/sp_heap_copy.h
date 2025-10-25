#ifndef SP_HEAP_COPY_H
#define SP_HEAP_COPY_H

#include <stddef.h>
#include <stdbool.h>

#include <sp_callbacks.h>

//==============================
struct sp_heap_copy;

//==============================
struct sp_heap_copy *
sp_heap_copy_init(size_t element_align,
                  size_t element_sz,
                  sp_cb_cmp,
                  sp_cb_copy);

struct sp_heap_copy *
sp_heap_copy_init2(size_t element_align, size_t element_sz, sp_cb_cmp);

struct sp_heap_copy *
sp_heap_copy_init_copy(const struct sp_heap_copy *);

int
sp_heap_copy_free(struct sp_heap_copy **);

//==============================
sp_T *
sp_heap_copy_enqueue(struct sp_heap_copy *, const sp_T *in);

//==============================
bool
sp_heap_copy_dequeue(struct sp_heap_copy *, sp_T *dest);

//==============================
sp_T *
sp_heap_copy_head(struct sp_heap_copy *);

//==============================
bool
sp_heap_copy_drop_head(struct sp_heap_copy *);

//==============================
bool
sp_heap_copy_remove_impl(struct sp_heap_copy *, sp_T *);

//==============================
sp_T *
sp_heap_copy_update_key(struct sp_heap_copy *, sp_T *);

//==============================
size_t
sp_heap_copy_length(const struct sp_heap_copy *);

size_t
sp_heap_copy_capacity(const struct sp_heap_copy *);

//==============================
bool
sp_heap_copy_eq(const struct sp_heap_copy *,
                const struct sp_heap_copy *,
                sp_cb_eq eq);

//==============================
bool
sp_heap_copy_is_empty(const struct sp_heap_copy *);

//==============================
sp_T *
sp_heap_copy_array(struct sp_heap_copy *);

//==============================
int
sp_heap_copy_clear(struct sp_heap_copy *);

//==============================

#endif
