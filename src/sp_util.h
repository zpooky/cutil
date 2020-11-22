#ifndef _SP_UTIL_H
#define _SP_UTIL_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

//==============================
#define sp_util_max(a, b) (((a) > (b)) ? (a) : (b))

#define sp_util_min(a, b) (((a) < (b)) ? (a) : (b))

//==============================
#define SP_KB 1024

//==============================
void
sp_util_to_hex(const char *ctx, const uint8_t *raw, size_t len);

//==============================
const uint8_t *
sp_util_hex_encode(const uint8_t *it,
                   const uint8_t *const end,
                   char *out,
                   size_t l_out);
const char *
sp_util_hex_decode(const char *hex, size_t lhex, uint8_t *out, size_t lout);

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

uint64_t
sp_util_ntohll(uint64_t n);

//==============================
int sp_util_size_t_cmp(size_t, size_t);

int sp_util_uint32_cmp(uint32_t, uint32_t);

int
sp_util_void_cmp(const void *, const void *);

//==============================
typedef struct sp_pair {
  void *first;
  void *second;
} sp_pair;

struct sp_pair *
sp_pair_init(void *, void *);

int
sp_pair_free(struct sp_pair **);

void
sp_pair_set(sp_pair *dest, sp_pair *src);

//==============================
void
sp_util_std_flush(void);

#define assertx assert
#ifdef NDEBUG
#define assertx_n(n) (n)
#else
#define assertx_n(n)                                                           \
  do {                                                                         \
    sp_util_std_flush();                                                       \
    assert(n);                                                                 \
  } while (0)
#endif

//==============================
typedef int (*sp_util_sort_cmp_cb)(void *, void *);

void
sp_util_sort(void **, size_t, sp_util_sort_cmp_cb);

//==============================
bool
sp_util_is_printable(const uint8_t *b, size_t len);

//==============================
#endif
