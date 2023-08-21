#include "sp_str_util.h"

#include "sp_util.h"

#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <memory.h>

// ========================================
bool
sp_str_util_ends_with0(const char *str, size_t l_str, const char *suffix)
{
  size_t l_suffix = strlen(suffix);
  size_t offset;
  if (l_suffix > l_str) {
    return false;
  }
  offset = l_str - l_suffix;

  return memcmp(str + offset, suffix, l_suffix) == 0;
}

bool
sp_str_util_ends_with(const char *str, const char *suffix)
{
  size_t l_str = strlen(str);
  return sp_str_util_ends_with0(str, l_str, suffix);
}

// ========================================
int
sp_str_util_append(char *dest,
                   size_t dest_capacity,
                   size_t *l_dest,
                   const char *src,
                   size_t l_src)
{
  if ((*l_dest + l_src) >= dest_capacity) {
    return -1;
  }
  memcpy(dest + *l_dest, src, l_src);
  *l_dest += l_src;
  dest[*l_dest] = '\0';
  return 0;
}

int
sp_str_util_append_char(char *dest,
                        size_t dest_capacity,
                        size_t *l_dest,
                        char src)
{
  return sp_str_util_append(dest, dest_capacity, l_dest, &src, 1);
}

// ========================================
#define CONVERT(in, out, out_type, out_min, out_max)                           \
  do {                                                                         \
    char *endptr      = NULL;                                                  \
    long long int tmp = strtoll(in, &endptr, 10);                              \
    if (errno == ERANGE && (tmp == LLONG_MAX || tmp == LLONG_MIN)) {           \
      return false;                                                            \
    } else if (tmp == 0 && errno != 0) {                                       \
      return false;                                                            \
    } else if (endptr == in) {                                                 \
      return false;                                                            \
    }                                                                          \
    if (tmp > out_max || tmp < out_min) {                                      \
      return false;                                                            \
    }                                                                          \
    *out = (out_type)tmp;                                                      \
    return true;                                                               \
  } while (0)

#define CONVERTu(in, out, out_type, out_max)                                   \
  do {                                                                         \
    char *endptr      = NULL;                                                  \
    long long int tmp = strtoll(in, &endptr, 10);                              \
    if (errno == ERANGE && (tmp == LLONG_MAX || tmp == LLONG_MIN)) {           \
      return false;                                                            \
    } else if (tmp == 0 && errno != 0) {                                       \
      return false;                                                            \
    } else if (endptr == in) {                                                 \
      return false;                                                            \
    }                                                                          \
    if (tmp < 0) {                                                             \
      return false;                                                            \
    }                                                                          \
    *out = (out_type)tmp;                                                      \
    if (*out > out_max) {                                                      \
      *out = 0;                                                                \
      return false;                                                            \
    }                                                                          \
    return true;                                                               \
  } while (0)

bool
sp_str_to_ll(const char *in, long long *out)
{
  assertx(in);
  assertx(out);
  CONVERT(in, out, long long, LLONG_MIN, LLONG_MAX);
}

bool
sp_str_to_ull(const char *in, unsigned long long *out)
{
  assertx(in);
  assertx(out);
  CONVERTu(in, out, unsigned long long, ULLONG_MAX);
}

bool
sp_str_to_ul(const char *in, unsigned long *out)
{
  assertx(in);
  assertx(out);
  CONVERTu(in, out, unsigned long, ULLONG_MAX);
}

bool
sp_str_to_ui(const char *in, unsigned int *out)
{
  assertx(in);
  assertx(out);
  CONVERTu(in, out, unsigned int, UINT_MAX);
}

bool
sp_str_to_ull_max(const char *in,
                  unsigned long long *out,
                  unsigned long long max)
{
  assertx(in);
  assertx(out);
  CONVERTu(in, out, unsigned long long, max);
}

bool
sp_str_to_ul_max(const char *in, unsigned long *out, unsigned long max)
{
  assertx(in);
  assertx(out);
  CONVERTu(in, out, unsigned long, max);
}

bool
sp_str_to_ui_max(const char *in, unsigned int *out, unsigned int max)
{
  assertx(in);
  assertx(out);
  CONVERTu(in, out, unsigned int, max);
}

// ========================================
