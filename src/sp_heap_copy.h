#ifndef SP_HEAP_COPY_H
#define SP_HEAP_COPY_H

#include <stddef.h>
#include <stdbool.h>

//==============================
struct sp_heap_copy;

typedef void sp_heap_copy_T;

typedef int (*sp_heap_copy_cmp_cb)(sp_heap_copy_T *f, sp_heap_copy_T *s);
typedef void (*sp_heap_copy_copy_cb)(sp_heap_copy_T *dest,
                                     const sp_heap_copy_T *src,
                                     size_t);

//==============================
struct sp_heap_copy *
sp_heap_copy_init(size_t align,
                  size_t sz,
                  sp_heap_copy_cmp_cb,
                  sp_heap_copy_copy_cb);

struct sp_heap_copy *
sp_heap_copy_init2(size_t align, size_t sz, sp_heap_copy_cmp_cb);

int
sp_heap_copy_free(struct sp_heap_copy **);

//==============================
sp_heap_copy_T *
sp_heap_copy_enqueue(struct sp_heap_copy *, const sp_heap_copy_T *in);

//==============================
bool
sp_heap_copy_dequeue(struct sp_heap_copy *, sp_heap_copy_T *dest);

//==============================
sp_heap_copy_T *
sp_heap_copy_head(struct sp_heap_copy *);

//==============================
bool
sp_heap_copy_drop_head(struct sp_heap_copy *);

//==============================
bool
sp_heap_copy_remove_impl(struct sp_heap_copy *, sp_heap_copy_T *);

//==============================
sp_heap_copy_T *
sp_heap_copy_update_key(struct sp_heap_copy *, sp_heap_copy_T *);

//==============================
size_t
sp_heap_copy_length(const struct sp_heap_copy *);

size_t
sp_heap_copy_capacity(const struct sp_heap_copy *);

//==============================
bool
sp_heap_copy_is_empty(const struct sp_heap_copy *);

//==============================
sp_heap_copy_T *
sp_heap_copy_array(struct sp_heap_copy *);

//==============================
int
sp_heap_copy_clear(struct sp_heap_copy *);

//==============================

#endif
