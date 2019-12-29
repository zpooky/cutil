#ifndef _SP_VECTOR_H
#define _SP_VECTOR_H

#include <stddef.h>
#include <stdbool.h>

//==============================
struct sp_vec;

typedef void sp_vec_T;

typedef int (*sp_vec_cmp_cb)(sp_vec_T *, sp_vec_T *);
typedef int (*sp_vec_it_cb)(sp_vec_T *, void *closure);

//==============================
struct sp_vec *
sp_vec_init(void);

struct sp_vec *
sp_vec_init_cap(size_t capacity);

int
sp_vec_free(struct sp_vec **);

//==============================
size_t
sp_vec_length(const struct sp_vec *);

size_t
sp_vec_capacity(const struct sp_vec *);

//==============================
bool
sp_vec_is_empty(const struct sp_vec *);

//==============================
sp_vec_T *
sp_vec_get(struct sp_vec *self, size_t);

const sp_vec_T *
sp_vec_get_c(const struct sp_vec *self, size_t);

sp_vec_T *
sp_vec_get_last(struct sp_vec *self);

const sp_vec_T *
sp_vec_get_last_c(const struct sp_vec *self);

//==============================
sp_vec_T **
sp_vec_append_impl(struct sp_vec *self, sp_vec_T *);

#define sp_vec_append(self, in) ((typeof(in) *)sp_vec_append_impl((self), (in)))

int
sp_vec_append_vec(struct sp_vec *self, struct sp_vec *);

//==============================
sp_vec_T *
sp_vec_remove(struct sp_vec *self, size_t);

//==============================
bool
sp_vec_swap(struct sp_vec *self, size_t f, size_t s);

//==============================
size_t
sp_vec_index_of_impl(const struct sp_vec *self, sp_vec_T **);

#define sp_vec_index_of(self, element)                                         \
  sp_vec_index_of_impl(self, (sp_vec_T **)element);

//==============================
int
sp_vec_sort(struct sp_vec *self, sp_vec_cmp_cb);

//==============================
int
sp_vec_for_each(struct sp_vec *self, void *closure, sp_vec_it_cb);

//==============================
int
sp_vec_clear(struct sp_vec *self);

//==============================
sp_vec_T **
sp_vec_array(struct sp_vec *);

//==============================
sp_vec_T **
sp_vec_begin(struct sp_vec *);

sp_vec_T **
sp_vec_end(struct sp_vec *);

#define sp_vec_for_each2(it, self)                                             \
  for (it = (typeof(it))sp_vec_begin(self);                                    \
       (sp_vec_T **)it != sp_vec_end(self);                                    \
       it = (typeof(it))(((sp_vec_T **)it) + 1))

//==============================
#endif
