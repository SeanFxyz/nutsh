#ifndef _darray_h
#define _darray_h

#include <stdlib.h>
#include <string.h>
#include "dbg.h"

#define DARR_DEFAULT_EX 10

typedef struct darr {
	int len;
	int cap; // Current capacity of character array
	size_t el_size;
	int ex; // Extra space to leave for new characters when resizing
	void **contents;
} darr;

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

darr *darr_create(size_t el_size, size_t init_cap)
{
	darr *arr = malloc(sizeof(darr));
	check_mem(arr);
	arr->cap = init_cap;
	check(arr->cap > 0, "Initial cap must be > 0");

	arr->contents = calloc(init_cap, sizeof(void *));
	check_mem(arr->contents);

	arr->len = 0;
	arr->el_size = el_size;
	arr->ex = DARR_DEFAULT_EX;

	return arr;
}

void darr_clear(darr *arr)
{
	int i;
	for (i=0; i < arr->len; i++) {
		if (arr->contents[i] != NULL) {
			free(arr->contents[i]);
			// arr->contents[i] = NULL;
		}
	}
}

static inline int darr_resize(darr *arr, size_t newsize)
{
	check(newsize > 0, "darr_resize: new size must be > 0");
	arr->cap = newsize;

	if (arr->cap < arr->len)
		arr->len = arr->cap;

	void *contents = realloc(arr->contents, arr->cap * sizeof(void *));
	check_mem(contents);

	arr->contents = contents;

	return 0;
}

int darr_expand(darr *arr)
{
	size_t old_cap = arr->cap;
	check(darr_resize(arr, arr->cap + arr->ex) == 0,
			"Failed to expand array");

	memset(arr->contents + old_cap, 0, arr->ex + 1);
	return 0;
}

int darr_contract(darr *arr)
{
	int new_size = arr->len < (int)arr->ex ? (int)arr->ex : arr->len;

	return darr_resize(arr, new_size + 1);
}

void darr_del(darr *arr)
{
	if (arr) {
		if (arr->contents)
			free(arr->contents);
		free(arr);
	}
}

void darr_clear_del(darr *arr)
{
	darr_clear(arr);
	darr_del(arr);
}

int darr_push(darr *arr, void *el)
{
	check(arr->el_size == sizeof(el), "darr_push: wrong el_size");
	arr->contents[arr->len] = el;
	arr->len++;

	if (arr->len >= arr->cap)
		return darr_expand(arr);
	else
		return 0;
}

void *darr_pop(darr *arr)
{
	check(arr->len - 1 >= 0, "darr_pop: array empty");

	void *el = darr_remove(arr, arr->len - 1);
	arr->len--;

	if (arr->len > (int)arr->ex && arr->len % arr->ex)
		darr_contract(arr);

	return el;
}
#endif
