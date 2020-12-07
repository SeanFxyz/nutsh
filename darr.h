#ifndef _DARR_H
#define _DARR_H

#include <stdlib.h>
#include <string.h>
#include "dbg.h"

#define DARR_DEFAULT_EX 30

/*
 * Dynamic array mostly based on the example implementation from
 * Learn C the Hard Way by Zed Shaw
 */

typedef struct darr {
	int len;
	int cap; // Current capacity of character array
	size_t el_size;
	int ex; // Extra space to leave for new characters when resizing
	void **data;
} darr;

darr *darr_create(size_t el_size, size_t init_cap);
void darr_del(darr *arr);
int darr_expand(darr *arr);
int darr_contract(darr *arr);
int darr_contract_full(darr *arr);
int darr_push(darr *arr, void *el);
void *darr_pop(darr *arr);
void darr_clear_del(darr *arr);

static inline darr *darr_new(darr *arr)
{
	return calloc(1, arr->el_size);
}

static inline void darr_set(darr *arr, int i, void *el)
{
	if (i >= arr->len)
		arr->len = i + 1;
	arr->data[i] = el;
}

static inline void *darr_get(darr *arr, int i)
{
	return arr->data[i];
}

static inline void *darr_remove(darr *arr, int i)
{
	void *el = arr->data[i];
	arr->data[i] = NULL;
	return el;
}

#endif
