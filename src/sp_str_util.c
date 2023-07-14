#include "sp_str_util.h"
#include <stddef.h>
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
