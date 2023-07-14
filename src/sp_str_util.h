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

#endif
