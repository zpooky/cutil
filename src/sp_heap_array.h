#ifndef SP_HEAP_ARRAY_H
#define SP_HEAP_ARRAY_H

#include <stddef.h>
#include <sp_callbacks.h>

sp_T *
sp_heap_array_enqueue_move(void *array,
                           size_t capacity,
                           size_t *l_array,
                           size_t esize,
                           sp_cb_cmp,
                           sp_T *in);

#endif
