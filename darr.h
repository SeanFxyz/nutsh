#ifndef _DARR_H
#define _DARR_H

#include <stdlib.h>
#include <string.h>
#include "dbg.h"

#define DARR_DEFAULT_EX 30

typedef struct darr {
	int len;
	int cap; // Current capacity of character array
	size_t el_size;
	int ex; // Extra space to leave for new characters when resizing
	void **contents;
} darr;

darr *darr_create(size_t el_size, size_t init_cap);
void darr_del(darr *arr);
int darr_expand(darr *arr);
int darr_contract(darr *arr);
int darr_push(darr *arr, void *el);
void *darr_pop(darr *arr);
void darr_clear_destroy(darr *arr);

static inline darr *darr_new(darr *arr)
{
	check(arr->el_size > 0, "Can't use darr_new on 0 size darr");
	return calloc(1, arr->el_size);
}

static inline void darr_set(darr *arr, int i, void *el)
{
	check(i >= 0, "darr set out of bounds");
	if (i >= arr->len)
		arr->len = i + 1;
	arr->contents[i] = el;
}

static inline void *darr_get(darr *arr, int i)
{
	check(i >= 0 && i < arr->len, "darr get out of bounds");
	return arr->contents[i];
}

static inline void *darr_remove(darr *arr, int i)
{
	void *el = arr->contents[i];
	arr->contents[i] = NULL;
	return el;
}

#endif
