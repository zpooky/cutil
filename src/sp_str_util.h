#ifndef _SP_STRING_UTIL_H
#define _SP_STRING_UTIL_H

#include <stdbool.h>
#include <stddef.h>

// ========================================
bool
sp_str_util_ends_with0(const char *str, size_t l_str, const char *suffix);

bool
sp_str_util_ends_with(const char *str, const char *suffix);

/* bool
 * sp_str_util_ends_withi(const char *str, const char *suffix);
 */

// ========================================
int
sp_str_util_append(char *dest,
                   size_t dest_capacity,
                   size_t *l_dest,
                   const char *src,
                   size_t l_src);

int
sp_str_util_append_char(char *dest,
                        size_t dest_capacity,
                        size_t *l_dest,
                        char src);

// ========================================

#endif
