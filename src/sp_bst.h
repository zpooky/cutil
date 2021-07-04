#ifndef _SP_BST_H
#define _SP_BST_H

#include <stdbool.h>
#include <stddef.h>

//==============================
struct sp_bst;

typedef struct sp_bst_Node {
  struct sp_bst_Node *left;
  struct sp_bst_Node *right;
} sp_bst_Node;

typedef sp_bst_Node sp_bst_T;

/* static int example_cmp(struct example *f, struct example *s) {
 *   int res = sp_util_uint32_cmp(f->first, s->first);
 *   if (res == 0) res = sp_util_uint32_cmp(f->second, s->second);
 *   return res;
 * }
 */
typedef int (*sp_bst_node_cmp_cb)(sp_bst_T *, sp_bst_T *);
/* static struct sp_bst_Node *example_new(struct example *in){
 *   struct example *result;
 *   if((result = calloc(1, sizeof(*result)))){
 *     #<{(| init $result based on $in |)}>#
 *   }
 *   return &result->base;
 * }
 */
typedef struct sp_bst_Node *(*sp_bst_node_new_cb)(sp_bst_T *);
/* static int example_free(struct example *in){
 *   free(in);
 *   return 0;
 * }
 */
typedef int (*sp_bst_node_free_cb)(sp_bst_T *);

//==============================
typedef int (*sp_bst_node_it_cb)(sp_bst_T *, void *);

//==============================
/* struct sp_bst *tree = sp_bst_init((sp_bst_node_cmp_cb)example_cmp,
 *                                   (sp_bst_node_new_cb)example_new,
 *                                   (sp_bst_node_free_cb)example_free);
 */
struct sp_bst *
  sp_bst_init(sp_bst_node_cmp_cb, sp_bst_node_new_cb, sp_bst_node_free_cb);

//==============================
typedef struct sp_bst_voidp_Node {
  struct sp_bst_Node base;
  void *value;
} sp_bst_voidp_Node;

/* Default impl for void* where the pointer is used as the id */
struct sp_bst *
sp_bst_voidp_init(void);

struct sp_bst *
sp_bst_voidp_init_cmp(sp_bst_node_cmp_cb cmp);

struct sp_bst *
sp_bst_init_identity(sp_bst_node_cmp_cb cmp);

//==============================
bool
sp_bst_is_empty(const struct sp_bst *);

//==============================
size_t
sp_bst_length(const struct sp_bst *);

//==============================
sp_bst_T *
sp_bst_insert_impl(struct sp_bst *, sp_bst_T *);

#define sp_bst_insert(self, in)                                                \
  ((typeof(in))sp_bst_insert_impl((self), &(in)->base))

sp_bst_T *
sp_bst_insert2_impl(struct sp_bst *, sp_bst_T *, sp_bst_node_new_cb);

#define sp_bst_insert2(self, in, new_cb)                                       \
  ((typeof(in))sp_bst_insert2_impl((self), &(in)->base), new_cb)

sp_bst_T *
sp_bst_insert_identity_impl(struct sp_bst *, sp_bst_T *);

#define sp_bst_insert_identity(self, in)                                       \
  ((typeof(in))sp_bst_insert_identity_impl((self), &(in)->base))

//==============================
sp_bst_T *
sp_bst_find_impl(struct sp_bst *, sp_bst_T *needle);

#define sp_bst_find(self, needle)                                              \
  ((typeof(needle))sp_bst_find_impl((self), &(needle)->base))

//==============================
sp_bst_T *
sp_bst_remove_impl(struct sp_bst *, sp_bst_T *needle);

#define sp_bst_remove(self, needle)                                            \
  ((typeof(needle))sp_bst_remove_impl((self), &(needle)->base))

bool
sp_bst_remove_free_impl(struct sp_bst *, sp_bst_T *needle);

#define sp_bst_remove_free(self, needle)                                       \
  sp_bst_remove_free_impl((self), &(needle)->base)

//==============================
int
sp_bst_clear(struct sp_bst *);

//==============================
int
sp_bst_free(struct sp_bst **);

//==============================
int
sp_bst_in_order(struct sp_bst *, void *, sp_bst_node_it_cb);

int
sp_bst_reverse_order(struct sp_bst *, void *, sp_bst_node_it_cb);

int
sp_bst_pre_order(struct sp_bst *, void *, sp_bst_node_it_cb);

int
sp_bst_post_order(struct sp_bst *, void *, sp_bst_node_it_cb);

//==============================
int
sp_bst_rebalance(struct sp_bst *);

//==============================
typedef struct sp_bst_It {
  void *queue;
  void *head;
} sp_bst_It;

void
sp_bst_begin(struct sp_bst *, struct sp_bst_It *);

void
sp_bst_next(struct sp_bst_It *);

//TODO break does not work
/* sp_bst_It it;
 * sp_bst_for_each (&it, bst) {
 *   struct example*entry = it.head;
 * }
 */
#define sp_bst_for_each(it, self)                                              \
  for (sp_bst_begin((self), (it)); (it)->head; sp_bst_next((it)))

//==============================
struct sp_bst_Node *
sp_bst_identity_new_cb(struct sp_bst_Node *in);

int
sp_bst_free_cb(sp_bst_T *);

//==============================

#endif
