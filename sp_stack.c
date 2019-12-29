#include "sp_stack.h"
#include <stdlib.h>
#include <assert.h>

//==============================
struct sp_stack {
  struct sp_stack_T *data;
  struct sp_stack *tail;
};

//==============================
bool
sp_stack_push(struct sp_stack /*<T>*/ **self, sp_stack_T *data)
{
  struct sp_stack *head;

  assert(self);
  assert(data);

  if ((head = calloc(1, sizeof(*head)))) {
    head->data = data;
    head->tail = *self;
    *self = head;
  }

  return head;
}

//==============================
bool
sp_stack_pop_impl(struct sp_stack /*<T>*/ **self, sp_stack_T **out)
{
  sp_stack_T *result = NULL;

  assert(self);
  assert(out);

  if (*self) {
    struct sp_stack *tail = (*self)->tail;
    result = (*self)->data;
    free(*self);
    *self = tail;
  }

  *out = result;

  return result != NULL;
}

//==============================
bool
sp_stack_is_empty(const struct /*<T>*/ sp_stack *self)
{
  return self == NULL;
}

//==============================
int
sp_stack_free(struct sp_stack /*T*/ **pself)
{
  sp_stack_T *tmp = NULL;
  while (sp_stack_pop(pself, &tmp))
    ;

  return 0;
}

//==============================
