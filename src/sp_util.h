#ifndef _SP_UTIL_H
#define _SP_UTIL_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

//==============================
#define sp_max(a, b) (((a) > (b)) ? (a) : (b))

#define sp_min(a, b) (((a) < (b)) ? (a) : (b))

#define SP_ARRAY_LEN(arr) (sizeof(arr) / sizeof(arr[0]))

//==============================
#define SP_KB 1024

//==============================
void
sp_util_to_hex(void *stream, const char *ctx, const void *raw, size_t len);

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
sp_util_swap_uint8_t(uint8_t *, uint8_t *);

void
sp_util_swap_int8_t(int8_t *, int8_t *);

void
sp_util_swap_uint16_t(uint16_t *, uint16_t *);

void
sp_util_swap_int16_t(int16_t *, int16_t *);

void
sp_util_swap_uint32_t(uint32_t *, uint32_t *);

void
sp_util_swap_int32_t(int32_t *, int32_t *);

void
sp_util_swap_uint64_t(uint64_t *, uint64_t *);

void
sp_util_swap_int64_t(int64_t *, int64_t *);

void
sp_util_swap_char(char *, char *);

void
sp_util_swap_char_arr(char *, char *, size_t);

void
sp_util_swap_raw(void *, void *, size_t);

//==============================
uint64_t sp_util_htonll(uint64_t);

uint64_t
sp_util_ntohll(uint64_t n);

//==============================
#define SP_UTIL_NUMERIC_CMP(a, b) ((int)((a) - (b)))
int sp_util_size_t_cmp(size_t, size_t);

int sp_util_uint16_cmp(uint16_t, uint16_t);

int sp_util_uint32_cmp(uint32_t, uint32_t);

int
sp_util_uint16p_cmp(const uint16_t *, const uint16_t *);

int
sp_util_uint32p_cmp(const uint32_t *, const uint32_t *);

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
__sp_dump_stack_impl(void *dest,
                     const char *file,
                     const char *func,
                     unsigned line);

#define sp_dump_stack()                                                        \
  __sp_dump_stack_impl(stderr, __FILE__, __func__, __LINE__)

//==============================
void
__sp_util_std_flush(void);

void
__sp_util_assert(const char *file,
                 unsigned int line,
                 const char *proto,
                 const char *cond);

#ifdef NDEBUG
#define assertx_n(n) (n)
#define assertx(__e) ((void)0)
#define assertxs(__e, ...) ((void)0)
#else
#define assertx_n(__e)                                                         \
  do {                                                                         \
    __sp_util_std_flush();                                                     \
    assertx(__e);                                                              \
  } while (0)

#define assertx(__e)                                                           \
  if (!(__e)) {                                                                \
    __sp_util_assert(__FILE__, __LINE__, __func__, #__e);                      \
  }
#define assertxs(__e, ...)                                                     \
  if (!(__e)) {                                                                \
    fprintf(stderr, "\n" __VA_ARGS__);                                         \
    __sp_util_assert(__FILE__, __LINE__, __func__, #__e);                      \
  }
#endif

//==============================
typedef int (*sp_util_sort_cmp_cb)(const void *, const void *);
typedef void (*sp_util_sort_swap_cb)(void *, void *, size_t sz);

bool
sp_util_is_sorted(const void *arr,
                  size_t arr_len,
                  size_t entry_sz,
                  sp_util_sort_cmp_cb);

void
sp_util_sort0(void *arr,
              size_t arr_len,
              size_t entry_sz,
              sp_util_sort_cmp_cb,
              sp_util_sort_swap_cb swap);

void
sp_util_sort(void *arr, size_t arr_len, size_t entry_sz, sp_util_sort_cmp_cb);

void
sp_util_sort_ptr_arr(void **, size_t, sp_util_sort_cmp_cb);

//==============================
bool
sp_util_is_printable(const uint8_t *b, size_t len);

//==============================
size_t
sp_util_align(size_t v, size_t align);

//==============================
void *
sp_util_bin_search(void *arr,
                   size_t arr_len,
                   void *needle,
                   size_t needle_sz,
                   sp_util_sort_cmp_cb cmp);

//==============================
size_t
sp_util_bin_insert_uniq0(void *arr,
                         size_t *arr_len,
                         const void *in,
                         size_t in_size,
                         sp_util_sort_cmp_cb cmp,
                         sp_util_sort_swap_cb swap);
size_t
sp_util_bin_insert_uniq(void *arr,
                        size_t *arr_len,
                        const void *in,
                        size_t in_size,
                        sp_util_sort_cmp_cb cmp);

//==============================
bool
sp_util_parse_uint(const char *str,
                   const char *str_end,
                   unsigned long long *out);

bool
sp_util_parse_int(const char *str, const char *str_end, signed long long *out);

//==============================
typedef void (*sp_util_copy_cb)(void *dest, const void *src, size_t);
void
sp_util_memcopy(void *dest, const void *src, size_t sz);

//==============================
bool
sp_util_close(int *fd);

//==============================
#endif
