#ifndef _darray_h
#define _darray_h

#include <assert.h>

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

void darr_clear(darr *arr);
{
	for (int i=0; i < arr->len; i++) {
		if (arr->contents[i] != NULL)
			free(arr->contents[i]);
	}
}

int darr_resize(darr *arr, size_t newsize)
{
	check(newsize > 0, "darr_resize: new size must be > 0");

	arr->cap = newsize;
	if (arr->cap < arr->len)
		arr->len = arr->cap;

	void *contents = realloc(arr->contents, arr->cap * sizeof(void *));
}

int darr_expand(darr *arr)
{
	arr->cap = arr->cap + ex;
	void **newcontents = malloc(sizeof(void *) * arr->cap);
	for(int i=0; i < arr->len; i++)
		darr_push(arr, arr->contents[i]);
}

int darr_contract

void *darr_remove(darr *arr, int i)
{
	void *el = arr->contents[i];
	arr->contents[i] = NULL;
	return el;
}

int darr_push(darr *arr, void *el)
{
	check(arr->el_size == sizeof(el), "darr_push: wrong el_size");
	arr->content[len] = el;
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
	arr->end--;

	if (arr->len > (int)arr->ex && arr->len % arr->ex)
		darr_contract(arr);

	return el;
}

void darr_clear_del(darr *arr)
{
	darr_clear(arr);
	darr_del(arr);
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

inline void *darr_get(darr *arr, int i)
{
	check(i >= 0 && i < arr->len, "darr get out of bounds");
	return arr->contents[i];
}
#endif
