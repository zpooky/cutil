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
