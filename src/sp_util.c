#include "sp_util.h"

#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include <execinfo.h> // backtrace

//==============================
static const char hex_encode_lookup[] = {
  '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
};
void
sp_util_to_hex(const char *ctx, const uint8_t *raw, size_t len)
{
  size_t i;

  if (ctx) {
    printf("%s ", ctx);
  }
  for (i = 0; i < len; ++i) {
    int first  = (raw[i] >> 4) & 0xf;
    int second = raw[i] & 0xf;
    printf("%c%c", hex_encode_lookup[first], hex_encode_lookup[second]);
  }
  printf("\n");
}

//==============================
const uint8_t *
sp_util_hex_encode(const uint8_t *it,
                   const uint8_t *const end,
                   char *out,
                   size_t l_out)
{
  assert(l_out > 0);
  --l_out;
  if (l_out) {
    const char *const eout = out + l_out - (l_out % 2);

    while (it != end && out != eout) {
      *out++ = hex_encode_lookup[(*it >> 4) & 0xf];
      *out++ = hex_encode_lookup[*it & 0xf];
      ++it;
    }
  }

  *out = '\0';
  return it;
}

const char *
sp_util_hex_decode(const char *it, size_t lhex, uint8_t *out, size_t lout)
{
  const char *const end = it + lhex;
  //TODO add indicate out length
  // TODO lowercase parse

  uint8_t lookup[('F' - '0') + 1];
  lookup['0' - '0'] = 0x0;
  lookup['1' - '0'] = 0x1;
  lookup['2' - '0'] = 0x2;
  lookup['3' - '0'] = 0x3;
  lookup['4' - '0'] = 0x4;
  lookup['5' - '0'] = 0x5;
  lookup['6' - '0'] = 0x6;
  lookup['7' - '0'] = 0x7;
  lookup['8' - '0'] = 0x8;
  lookup['9' - '0'] = 0x9;
  lookup['A' - '0'] = 0xA;
  lookup['B' - '0'] = 0xB;
  lookup['C' - '0'] = 0xC;
  lookup['D' - '0'] = 0xD;
  lookup['E' - '0'] = 0xE;
  lookup['F' - '0'] = 0xF;

  assert(lhex % 2 == 0);

  while (it != end) {
    int idxf = (int)(*it++ - '0');
    int idxs = (int)(*it++ - '0');
    assert(idxf < sizeof(lookup));
    assert(idxf >= 0);

    assert(idxs < sizeof(lookup));
    assert(idxs >= 0);

    uint8_t f = lookup[idxf];
    uint8_t s = lookup[idxs];
    *out++    = (f << 4) | s;
  }

  return it;
}

//==============================
void
sp_util_swap_voidp_impl(void **f, void **s)
{
  void *tmp = *f;
  *f        = *s;
  *s        = tmp;
}

void
sp_util_swap_voidpp(void ***f, void ***s)
{
  void **tmp = *f;
  *f         = *s;
  *s         = tmp;
}

void
sp_util_swap_size_t(size_t *f, size_t *s)
{
  size_t tmp = *f;
  *f         = *s;
  *s         = tmp;
}

void
sp_util_swap_uint8_t(uint8_t *f, uint8_t *s)
{
  uint8_t tmp = *f;
  *f          = *s;
  *s          = tmp;
}

void
sp_util_swap_int8_t(int8_t *f, int8_t *s)
{
  int8_t tmp = *f;
  *f         = *s;
  *s         = tmp;
}

void
sp_util_swap_uint16_t(uint16_t *f, uint16_t *s)
{
  uint16_t tmp = *f;
  *f           = *s;
  *s           = tmp;
}

void
sp_util_swap_int16_t(int16_t *f, int16_t *s)
{
  int16_t tmp = *f;
  *f          = *s;
  *s          = tmp;
}

void
sp_util_swap_uint32_t(uint32_t *f, uint32_t *s)
{
  uint32_t tmp = *f;
  *f           = *s;
  *s           = tmp;
}

void
sp_util_swap_int32_t(int32_t *f, int32_t *s)
{
  int32_t tmp = *f;
  *f          = *s;
  *s          = tmp;
}

void
sp_util_swap_char(char *f, char *s)
{
  char tmp = *f;
  *f       = *s;
  *s       = tmp;
}

void
sp_util_swap_char_arr(char *f, char *s, size_t len)
{
  size_t i;
  for (i = 0; i < len; ++i) {
    sp_util_swap_char(f + i, s + i);
  }
}

void
sp_util_swap_raw(void *f, void *s, size_t len)
{
  uint8_t tmp[len];
  memcpy(tmp, f, len);
  memcpy(f, s, len);
  memcpy(s, tmp, len);
}

//==============================
/* https://gist.github.com/hailinzeng/5604057 */
uint64_t
sp_util_htonll(uint64_t n)
{
#if __BYTE_ORDER == __BIG_ENDIAN
  return n;
#else
  uint32_t low  = (uint32_t)n;
  uint32_t high = (uint32_t)(n >> 32);

  return (((uint64_t)htonl(high)) << 32) + htonl(low);
#endif
}

uint64_t
sp_util_ntohll(uint64_t n)
{
#if __BYTE_ORDER == __BIG_ENDIAN
  return n;
#else
  uint32_t low  = (uint32_t)n;
  uint32_t high = (uint32_t)(n >> 32);

  return (((uint64_t)ntohl(high)) << 32) + ntohl(low);
#endif
}

//==============================
int
sp_util_size_t_cmp(size_t f, size_t s)
{
  if (f > s) {
    return 1;
  }
  if (f < s) {
    return -1;
  }

  return 0;
}

int
sp_util_uint16_cmp(uint16_t f, uint16_t s)
{
  if (f > s) {
    return 1;
  }
  if (f < s) {
    return -1;
  }

  return 0;
}

int
sp_util_uint32_cmp(uint32_t f, uint32_t s)
{
  if (f > s) {
    return 1;
  }
  if (f < s) {
    return -1;
  }

  return 0;
}

int
sp_util_uint16p_cmp(const uint16_t *f, const uint16_t *s)
{
  assert(f);
  assert(s);
  return sp_util_uint16_cmp(*f, *s);
}

int
sp_util_uint32p_cmp(const uint32_t *f, const uint32_t *s)
{
  assert(f);
  assert(s);
  return sp_util_uint32_cmp(*f, *s);
}

int
sp_util_void_cmp(const void *f, const void *s)
{
  assert(f);
  assert(s);

  intptr_t fi = (intptr_t)f;
  intptr_t si = (intptr_t)s;
  return fi - si;
}

//==============================
struct sp_pair *
sp_pair_init(void *first, void *second)
{
  struct sp_pair *result;

  if ((result = calloc(1, sizeof(*result)))) {
    result->first  = first;
    result->second = second;
  }

  return result;
}

int
sp_pair_free(struct sp_pair **pself)
{
  assert(pself);

  if (*pself) {
    free(*pself);
    *pself = NULL;
  }

  return 0;
}

void
sp_pair_set(sp_pair *dest, sp_pair *src)
{
  assert(dest);
  assert(src);

  dest->first  = src->first;
  dest->second = src->second;
}

//==============================
#define SP_CL_GREEN "\033[92m"
#define SP_CL_RESET "\033[0m"
void
sp_util_std_flush(void)
{
  /* printf("flush\n"); */
  fflush(stdout);
  fflush(stderr);
}

static void inline sp_util_backtrace(FILE *dest, const char *proto)
{
  const int BT_BUF_SIZE = 100;
  void *buffer[BT_BUF_SIZE];
  char **strings;
  int nptrs;

  nptrs = backtrace(buffer, BT_BUF_SIZE);
  if ((strings = backtrace_symbols(buffer, nptrs))) {
    int i;

    for (i = 0; i < nptrs; i++) {
      if (strstr(strings[i], proto)) {
        fprintf(dest, SP_CL_GREEN "%s" SP_CL_RESET "\n", strings[i]);
      } else {
        fprintf(dest, "%s\n", strings[i]);
      }
    }

    free(strings);
  }
}

void
sp_util_assert(const char *file,
               unsigned int line,
               const char *proto,
               const char *cond)
{
  FILE *dest = stdout;

  fprintf(dest, "\nassertion failed: (%s)\n", cond);
  fprintf(dest,
          "%s"
          ":%s()"
          ":" SP_CL_GREEN "%d" SP_CL_RESET "\n\n",
          file, proto, line);

  sp_util_backtrace(dest, proto);
  sp_util_std_flush();

  /* raise(SIGABRT); */
  abort();
  exit(1);
  /* terminate(); */
}

//==============================
// partition(arr=[1,2,3], arr_len=3, entry_sz=sizeof(arr[0]), cmp)
bool
sp_util_is_sorted(const void *arr,
                  size_t arr_len,
                  size_t entry_sz,
                  sp_util_sort_cmp_cb cmp)
{
  if (arr_len > 0) {
    size_t i;
    for (i = 0; i < arr_len - 1; ++i) {
      const void *cur  = arr + (i * entry_sz);
      const void *next = arr + ((i + 1) * entry_sz);

      if (cmp(cur, next) > 0) {
        return false;
      }
    }
  }
  return true;
}

// partition(arr=[1,2,3], arr_len=3, entry_sz=sizeof(arr[0]), cmp)
static size_t
partition(void *arr,
          size_t arr_len,
          size_t entry_sz,
          sp_util_sort_cmp_cb cmp,
          sp_util_sort_swap_cb swap)
{
  void *head = arr;
  void *tail = arr + ((arr_len - 1) * entry_sz);

  void *const start = arr;
  void *const end   = arr + (arr_len * entry_sz);

  void *p = tail;

  assert(arr_len > 0);

  while (true) {
    while (head != end && cmp(head, p) < 0) {
      head += entry_sz;
    }

    while (tail != start && cmp(p, tail) < 0) {
      tail -= entry_sz;
    }

    if (head >= tail) {
      break;
    }

    /* Pivot needs to be the same even if its position gets swapped. */
    if (head == p) {
      p = tail;
    } else if (tail == p) {
      p = head;
    }

    swap(head, tail, entry_sz);
    head += entry_sz;
    tail -= entry_sz;
  } //while

  assert(head < end);
  assert(head >= start);

  assert(tail < end);
  assert(tail >= start);

  assert((head - arr) % entry_sz == 0);
  return (head - arr) / entry_sz;
}

static void
quicksort(void *arr,
          size_t arr_len,
          size_t entry_sz,
          sp_util_sort_cmp_cb cmp,
          sp_util_sort_swap_cb swap)
{
  if (arr_len > 1) {
    assert(arr);

    size_t pivot_idx = partition(arr, arr_len, entry_sz, cmp, swap);
    quicksort(arr, pivot_idx, entry_sz, cmp, swap);
    quicksort(arr + (pivot_idx * entry_sz), arr_len - pivot_idx, entry_sz, cmp,
              swap);
  }
}

void
sp_util_sort0(void *arr,
              size_t arr_len,
              size_t entry_sz,
              sp_util_sort_cmp_cb cmp,
              sp_util_sort_swap_cb swap)
{

  quicksort(arr, arr_len, entry_sz, cmp, swap);
}

void
sp_util_sort(void *arr,
             size_t arr_len,
             size_t entry_sz,
             sp_util_sort_cmp_cb cmp)
{
  sp_util_sort0(arr, arr_len, entry_sz, cmp, sp_util_swap_raw);
}

void
sp_util_sort_ptr_arr(void **arr, size_t arr_len, sp_util_sort_cmp_cb cmp)
{
  sp_util_sort(arr, arr_len, sizeof(*arr), cmp);
}

//==============================
bool
sp_util_is_printable(const uint8_t *b, size_t len)
{
  size_t i;
  for (i = 0; i < len; ++i) {
    if (!(isprint(b[i]))) {
      return false;
    }
  }
  return true;
}

//==============================
size_t
sp_util_align(size_t v, size_t align)
{
  assert(align % 8 == 0);
  return (v + (align - 1)) & -align;
}

//==============================
// arr=[1,2,3], arr_len=3, needle=&2, entry_sz=sizeof(2), cmp
void *
sp_util_bin_search(void *arr,
                   size_t arr_len,
                   void *needle,
                   size_t needle_sz,
                   sp_util_sort_cmp_cb cmp)
{
  while (arr_len > 0) {
    size_t mid_idx = arr_len / 2;
    void *mid      = arr + (mid_idx * needle_sz);
    int res        = cmp(needle, mid);
    if (res == 0) {
      return mid;
    }
    if (mid_idx == 0) {
      break;
    }

    if (res > 0) {
      arr = mid;
    } else if (res < 0) {
    }
    arr_len -= mid_idx;
  } //while

  return NULL;
}

//==============================
size_t
sp_util_bin_insert_uniq0(void *arr,
                         size_t *arr_len,
                         const void *in,
                         size_t in_size,
                         sp_util_sort_cmp_cb cmp,
                         sp_util_sort_swap_cb swap)
{
  size_t right = *arr_len;
  size_t left  = 0;
  size_t mid   = 0;
  size_t res   = 0;
  int eq       = 0;

  while (left < right) {
    mid = (left + right) / 2;
    eq  = cmp(in, arr + (mid * in_size));
    if (eq < 0) {
      right = mid;
    } else if (eq > 0) {
      left = mid + 1;
    } else {
      return mid;
    }
  } //while

  if (eq > 0) {
    mid++;
  }
  res = mid;

  void *src = arr + (*arr_len * in_size);
  memcpy(src, in, in_size);

  for (; mid < *arr_len; ++mid) {
    void *dest = arr + (mid * in_size);
    swap(dest, src, in_size);
  }
  ++(*arr_len);

  return res;
}

size_t
sp_util_bin_insert_uniq(void *arr,
                        size_t *arr_len,
                        const void *in,
                        size_t in_size,
                        sp_util_sort_cmp_cb cmp)
{
  return sp_util_bin_insert_uniq0(arr, arr_len, in, in_size, cmp,
                                  sp_util_swap_raw);
}

//==============================
