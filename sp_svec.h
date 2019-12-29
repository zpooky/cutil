#ifndef _SP_S_VECTOR_H
#define _SP_S_VECTOR_H

#include <stddef.h>
#include <stdbool.h>

//==============================
struct sp_svec;

typedef void sp_svec_T;

typedef int (*sp_svec_dtor)(sp_svec_T *);
typedef int (*sp_svec_move)(sp_svec_T *from, sp_svec_T *to);

//==============================
struct sp_svec *
sp_svec_init_impl(size_t a, size_t sz);

#define sp_svec_init(type) sp_svec_init_impl(alignof(type), sizeof(type))

struct sp_svec *
sp_svec_init_impl_cap(size_t a, size_t sz, size_t capacity);

#define sp_svec_init_cap(type, cap)                                            \
  sp_svec_init_impl(alignof(type), sizeof(type), cap)

int
sp_svec_free(struct sp_svec **);

//==============================
/* int
 * sp_svec_swap_self(struct sp_svec *, struct sp_svec *);
 */

//==============================
int
sp_svec_clear(struct sp_svec *);

//==============================
size_t
sp_svec_length(const struct sp_svec *);

size_t
sp_svec_capacity(const struct sp_svec *);

//==============================
bool
sp_svec_is_empty(const struct sp_svec *);

//==============================
sp_svec_T *
sp_svec_get(struct sp_svec *self, size_t);

//==============================
sp_svec_T *
sp_svec_append_impl(struct sp_svec *self, sp_svec_T *);

#define sp_svec_append(self, in) ((typeof(in))sp_svec_append_impl((self), (in)))

//==============================
int
sp_svec_remove(struct sp_svec *self, size_t);

//==============================
bool
sp_svec_swap(struct sp_svec *self, size_t f, size_t s);

//==============================

#endif
