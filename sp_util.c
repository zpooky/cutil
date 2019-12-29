#include "sp_util.h"

#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

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

  result         = calloc(1, sizeof(*result));
  result->first  = first;
  result->second = second;

  return result;
}

int
sp_pair_free(struct sp_pair **pself)
{
  assert(pself);

  free(*pself);
  *pself = NULL;

  return 0;
}

//==============================
