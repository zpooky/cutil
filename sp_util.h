#ifndef _SP_UTIL_H
#define _SP_UTIL_H

#include <stddef.h>
#include <stdint.h>

//==============================
#define sp_util_max(a, b) (((a) > (b)) ? (a) : (b))

#define sp_util_min(a, b) (((a) < (b)) ? (a) : (b))

//==============================
void
to_hex(const uint8_t *raw, size_t len);

//==============================
void
sp_util_swap_voidp_impl(void **, void **);

//TODO can we assert $f and $s is a ptr** ?
#define sp_util_swap_voidp(f, s) sp_util_swap_voidp_impl((void **)f, (void **)s)

void
sp_util_swap_voidpp(void ***, void ***);

void
sp_util_swap_size_t(size_t *, size_t *);

void
sp_util_swap_char(char *, char *);

void
sp_util_swap_char_arr(char *, char *, size_t);

//==============================
uint64_t sp_util_htonll(uint64_t);

//==============================
int sp_util_size_t_cmp(size_t, size_t);

int sp_util_uint32_cmp(uint32_t, uint32_t);

//==============================
typedef struct sp_pair {
  void *first;
  void *second;
} sp_pair;

struct sp_pair *
sp_pair_init(void *, void *);

int
sp_pair_free(struct sp_pair **);

//==============================
#define assertx assert
#ifdef	NDEBUG
#define assertx_n(n)(n)
#else
#define assertx_n(n) assert(n)
#endif

//==============================
#endif
