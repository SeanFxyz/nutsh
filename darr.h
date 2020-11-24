#ifndef _darray_h
#define _darray_h

#include <assert.h>
#include "dbg.h"

#define DARR_DEFAULT_EX 10

typedef struct darr {
	int len;
	int cap; // Current capacity of character array
	size_t el_size;
	int ex; // Extra space to leave for new characters when resizing
	void **contents;
} darr;

darr *darr_new(size_t el_size, size_t init_cap)
{
	check(el_size > 0, "Can't create new darr with el_size 0");

	darr *arr = malloc(sizeof(darr));
	arr->len = 0;
	arr->cap = init_cap;
	arr->el_size = el_size;
	arr->ex = DARR_DEFAULT_EX;
	arr->contents = malloc(sizeof(void *) * init_cap);

	return arr;
}

void darr_del(darr *arr)
{
	if (arr) {
		if (arr->contents)
			free(arr->contents);
		free(arr);
	}
}

void darr_clear(darr *arr)
{
	for (int i=0; i < arr->len; i++) {
		if (arr->contents[i] != NULL)
			free(arr->contents[i]);
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

int darr_contract(darr *arr)
{
	int new_size = arr->len < (int)arr->ex ? (int)arr->ex : arr->len;

	return darr_resize(arr, new_size + 1);
}

void *darr_remove(darr *arr, int i)
{
	void *el = arr->contents[i];
	arr->contents[i] = NULL;
	return el;
}

inline void *darr_get(darr *arr, int i)
{
	check(i >= 0 && i < arr->len, "darr get out of bounds");
	return arr->contents[i];
}

int darr_expand(darr *arr)
{
	size_t old_cap = arr->cap;
	check(darr_resize(arr, arr->cap + arr->ex) == 0,
			"Failed to expand array");

	memset(arr->contents + old_cap, 0, arr->ex + 1);
	return 0;
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
	check(arr->end - 1 >= 0, "darr_pop: array empty");

	void *el = darr_remove(arr, arr->len - 1);
	arr->len--;

	if (arr->len > (int)arr->ex && arr->len % arr->ex)
		darr_contract(arr);

	return el;
}

inline void darr_set(darr *arr, int i, void *el)
{
	check(i >= 0, "darr set out of bounds");
	if (i >= arr->len) {
		if (i >= arr->cap)
			darr_expand(arr);
		arr->len = i + 1;
	}
	arr->contents[i] = el;
}

void darr_clear_del(darr *arr)
{
	darr_clear(arr);
	darr_del(arr);
}
#endif
