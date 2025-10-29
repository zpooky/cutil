#include "sp_callbacks.h"
#include <string.h>
#include <sp_util.h>

//==============================
void
sp_cb_copy_memcopy(sp_T *dest, const sp_T *src, size_t element_sz)
{
  assertx(dest);
  assertx(src);
  memcpy(dest, src, element_sz);
}

void
sp_cb_move_memcopy(sp_T *dest, sp_T *src, size_t element_sz)
{
  assertx(dest);
  assertx(src);
  memcpy(dest, src, element_sz);
}

//==============================
