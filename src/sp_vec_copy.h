#ifndef _SP_VECTOR_COPY_H
#define _SP_VECTOR_COPY_H

#include <stddef.h>
#include <stdbool.h>

//==============================
struct sp_vec_copy;

typedef void sp_vec_copy_T;

//==============================
typedef void (*sp_vec_copy_copy_cb)(sp_vec_copy_T *dest,
                                    const sp_vec_copy_T *src,
                                    size_t);

struct sp_vec_copy *
sp_vec_copy_init(size_t align, size_t sz, sp_vec_copy_copy_cb);

struct sp_vec_copy *
sp_vec_copy_init0(size_t align, size_t sz);

struct sp_vec_copy *
sp_vec_copy_init0_cap(size_t capacity, size_t align, size_t sz);

struct sp_vec_copy *
sp_vec_copy_init_cap(size_t capacity,
                     size_t align,
                     size_t sz,
                     sp_vec_copy_copy_cb);

int
sp_vec_copy_free(struct sp_vec_copy **);

//==============================
size_t
sp_vec_copy_length(const struct sp_vec_copy *);

size_t
sp_vec_copy_capacity(const struct sp_vec_copy *);

//==============================
bool
sp_vec_copy_is_empty(const struct sp_vec_copy *);

//==============================
sp_vec_copy_T *
sp_vec_copy_get(struct sp_vec_copy *self, size_t);

const sp_vec_copy_T *
sp_vec_copy_get_c(const struct sp_vec_copy *self, size_t);

sp_vec_copy_T *
sp_vec_copy_get_last(struct sp_vec_copy *self);

const sp_vec_copy_T *
sp_vec_copy_get_last_c(const struct sp_vec_copy *self);

//==============================
sp_vec_copy_T *
sp_vec_copy_append_impl(struct sp_vec_copy *self, const sp_vec_copy_T *);

#define sp_vec_copy_append(self, in)                                           \
  ((typeof(in) *)sp_vec_copy_append_impl((self), (in)))

int
sp_vec_copy_append_vec(struct sp_vec_copy *self, struct sp_vec_copy *);

//==============================
void
sp_vec_copy_remove(struct sp_vec_copy *self, size_t);

//==============================
bool
sp_vec_copy_swap(struct sp_vec_copy *self, size_t f, size_t s);

//==============================
size_t
sp_vec_copy_index_of(const struct sp_vec_copy *self, sp_vec_copy_T *);

//==============================
typedef int (*sp_vec_copy_cmp_cb)(sp_vec_copy_T *, sp_vec_copy_T *);

int
sp_vec_copy_sort(struct sp_vec_copy *self, sp_vec_copy_cmp_cb);

//==============================
typedef int (*sp_vec_copy_it_cb)(sp_vec_copy_T *, void *closure);

int
sp_vec_copy_for_each(struct sp_vec_copy *self,
                     void *closure,
                     sp_vec_copy_it_cb);

//==============================
int
sp_vec_copy_clear(struct sp_vec_copy *self);

//==============================
sp_vec_copy_T *
sp_vec_copy_array(struct sp_vec_copy *);

//==============================
sp_vec_copy_T *
sp_vec_copy_begin(struct sp_vec_copy *);

sp_vec_copy_T *
sp_vec_copy_end(struct sp_vec_copy *);

sp_vec_copy_T *
sp_vec_copy_next(struct sp_vec_copy *, sp_vec_copy_T *);

#define sp_vec_copy_for_each2(self, it)                                        \
  for (it = (typeof(it))sp_vec_copy_begin(self);                               \
       (sp_vec_copy_T *)it != sp_vec_copy_end(self);                           \
       it = (typeof(it))sp_vec_copy_next(self, it))

//==============================

#endif
