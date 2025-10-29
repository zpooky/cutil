#ifndef SP_CALLBACKS_H
#define SP_CALLBACKS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef void sp_T;

typedef int (*sp_cb_cmp)(const sp_T *f, const sp_T *s);
typedef bool (*sp_cb_eq)(const sp_T *f, const sp_T *s, size_t sz);

typedef void (*sp_cb_copy)(sp_T *dest, const sp_T *src, size_t);
typedef void (*sp_cb_move)(sp_T *dest, sp_T *src, size_t sz);

typedef int (*sp_cb_it)(sp_T *, void *closure);

typedef void (*sp_cb_swap)(sp_T *, sp_T *, size_t sz);

typedef uint32_t (*sp_cb_hash)(const sp_T *);

void
sp_cb_copy_memcopy(sp_T *dest, const sp_T *src, size_t element_sz);

void
sp_cb_move_memcopy(sp_T *dest, sp_T *src, size_t element_sz);

#endif
