#ifndef _SP_VECTOR_COPY_H
#define _SP_VECTOR_COPY_H

#include <stddef.h>
#include <stdbool.h>
#include <sp_callbacks.h>

//==============================
struct sp_vec_copy;

//==============================
struct sp_vec_copy *
sp_vec_copy_init(size_t element_align, size_t element_sz, sp_cb_copy);

struct sp_vec_copy *
sp_vec_copy_init_copy(const struct sp_vec_copy *);

struct sp_vec_copy *
sp_vec_copy_init0(size_t element_align, size_t element_sz);

struct sp_vec_copy *
sp_vec_copy_init0_cap(size_t capacity, size_t element_align, size_t element_sz);

struct sp_vec_copy *
sp_vec_copy_init_cap(size_t capacity,
                     size_t element_align,
                     size_t element_sz,
                     sp_cb_copy);

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
sp_T *
sp_vec_copy_get(struct sp_vec_copy *self, size_t);

const sp_T *
sp_vec_copy_get_c(const struct sp_vec_copy *self, size_t);

sp_T *
sp_vec_copy_get_last(struct sp_vec_copy *self);

const sp_T *
sp_vec_copy_get_last_c(const struct sp_vec_copy *self);

//==============================
sp_T *
sp_vec_copy_append_impl(struct sp_vec_copy *self, const sp_T *);

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
sp_vec_copy_index_of(const struct sp_vec_copy *self, sp_T *);

//==============================
int
sp_vec_copy_sort(struct sp_vec_copy *self, sp_cb_cmp);

//==============================
int
sp_vec_copy_for_each(struct sp_vec_copy *self, void *closure, sp_cb_it);

//==============================
int
sp_vec_copy_clear(struct sp_vec_copy *self);

//==============================
sp_T *
sp_vec_copy_array(struct sp_vec_copy *);

//==============================
sp_T *
sp_vec_copy_begin(struct sp_vec_copy *);

sp_T *
sp_vec_copy_end(struct sp_vec_copy *);

sp_T *
sp_vec_copy_next(struct sp_vec_copy *, sp_T *);

#define sp_vec_copy_for_each2(self, it)                                        \
  for (it = (typeof(it))sp_vec_copy_begin(self);                               \
       (sp_T *)it != sp_vec_copy_end(self);                                    \
       it = (typeof(it))sp_vec_copy_next(self, it))

//==============================

#endif
