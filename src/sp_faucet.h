#ifndef _SP_FAUCET_H
#define _SP_FAUCET_H

#include <stddef.h>
#include <stdbool.h>

//==============================
struct sp_cbb;

//==============================
struct sp_faucet;

typedef int (*sp_faucet_fill_cb)(struct sp_cbb *, void *);

//==============================
struct sp_faucet *
sp_faucet_init(sp_faucet_fill_cb, size_t cap, void *arg);

//==============================
int
sp_faucet_free(struct sp_faucet **);

//==============================
size_t
sp_faucet_peek_front(struct sp_faucet *, void *, size_t);

//==============================
size_t
sp_faucet_pop_front(struct sp_faucet *, void *, size_t);

//==============================
bool
sp_faucet_read(struct sp_faucet *, void *, size_t);

//==============================
#endif
