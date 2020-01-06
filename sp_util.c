#include "sp_util.h"

#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//==============================
void
to_hex(const uint8_t *raw, size_t len)
{
  size_t i;

  char lookup[16];
  lookup[0x0] = '0';
  lookup[0x1] = '1';
  lookup[0x2] = '2';
  lookup[0x3] = '3';
  lookup[0x4] = '4';
  lookup[0x5] = '5';
  lookup[0x6] = '6';
  lookup[0x7] = '7';
  lookup[0x8] = '8';
  lookup[0x9] = '9';
  lookup[0xA] = 'A';
  lookup[0xB] = 'B';
  lookup[0xC] = 'C';
  lookup[0xD] = 'D';
  lookup[0xE] = 'E';
  lookup[0xF] = 'F';

  for (i = 0; i < len; ++i) {
    int first  = (raw[i] >> 4) & 0xf;
    int second = raw[i] & 0xf;
    printf("%c%c", lookup[first], lookup[second]);
  }
  printf("\n");
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
