#include "sp_util.h"

#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

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
sp_util_swap_int8_t(int8_t *f, int8_t *s){
  int8_t tmp = *f;
  *f          = *s;
  *s          = tmp;
}

void
sp_util_swap_uint16_t(uint16_t *f, uint16_t *s)
{
  uint16_t tmp = *f;
  *f          = *s;
  *s          = tmp;
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
  *f          = *s;
  *s          = tmp;
}

void
sp_util_swap_int32_t(int32_t *f, int32_t *s){
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
void
sp_util_std_flush(void)
{
  /* printf("flush\n"); */
  fflush(stdout);
  fflush(stderr);
}

//==============================
static size_t
partition(void **in, size_t length, sp_util_sort_cmp_cb cmp)
{
  void **head = in;
  void **tail = in + (length - 1);

  void **const start = in;
  void **const end   = in + length;

  void **p = tail;

  while (true) {
    while (head != end && cmp(*head, /*<*/ *p) == -1) {
      head++;
    }

    while (tail != start && cmp(*p, /*<*/ *tail) == -1) {
      tail--;
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

    sp_util_swap_voidp(head, tail);
    head++;
    tail--;
  }

  assert(head < end);
  assert(head >= start);

  assert(tail < end);
  assert(tail >= start);

  return head - in;
}

static void
quicksort(void **in, size_t length, sp_util_sort_cmp_cb cmp)
{
  if (length > 1) {
    assert(in);

    size_t pivot = partition(in, length, cmp);
    quicksort(in, pivot, cmp);
    quicksort(in + pivot, length - pivot, cmp);
  }
}
//
void
sp_util_sort(void **raw, size_t len, sp_util_sort_cmp_cb cmp)
{
  quicksort(raw, len, cmp);
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
