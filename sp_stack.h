#ifndef _SP_STACK_H
#define _SP_STACK_H

#include <stdbool.h>

//==============================
struct sp_stack;

typedef void sp_stack_T;

//==============================
bool
sp_stack_push(struct sp_stack /*<sp_stack_T>*/ **self, sp_stack_T *data);

//==============================
bool
sp_stack_pop_impl(struct sp_stack /*<sp_stack_T>*/ **self, sp_stack_T **out);

#define sp_stack_pop(self, out) sp_stack_pop_impl((self), (sp_stack_T **)(out))

//==============================
bool
sp_stack_is_empty(const struct /*<sp_stack_T>*/ sp_stack *);

//==============================
int
sp_stack_free(struct sp_stack /*<sp_stack_T>*/ **self);

//==============================
#endif
