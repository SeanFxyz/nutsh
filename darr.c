/*
 * Written by Sean M Fitzgerald
 * sean_m_fitzgerald@tutanota.com
 *
 * Based on example implementation from
 * Learn C the Hard Way by Zed Shaw
 */

#include "darr.h"

darr *darr_create(size_t el_size, size_t init_cap)
{
	darr *arr = malloc(sizeof(darr));
	check_mem(arr);
	arr->cap = init_cap;
	check(arr->cap > 0, "Initial cap must be > 0");

	arr->data = calloc(init_cap, sizeof(void *));
	check_mem(arr->data);

	arr->len = 0;
	arr->el_size = el_size;
	arr->ex = DARR_DEFAULT_EX;

	return arr;
}

void darr_clear(darr *arr)
{
	int i;
	for (i=0; i < arr->len; i++) {
		if (arr->data[i] != NULL) {
			free(arr->data[i]);
			// arr->data[i] = NULL;
		}
	}
}

static inline int darr_resize(darr *arr, size_t newsize)
{
	arr->cap = newsize;

	if (arr->cap < arr->len)
		arr->len = arr->cap;

	void *data = realloc(arr->data, arr->cap * sizeof(void *));
	check_mem(data);

	arr->data = data;

	return 0;
}

int darr_expand(darr *arr)
{
	size_t old_cap = arr->cap;
	check(darr_resize(arr, arr->cap + arr->ex) == 0,
			"Failed to expand array");

	memset(arr->data + old_cap, 0, arr->ex + 1);
	return 0;
}

int darr_contract(darr *arr)
{
	int new_size = arr->len < (int)arr->ex ? (int)arr->ex : arr->len;

	return darr_resize(arr, new_size + 1);
}

int darr_contract_full(darr *arr)
{
	return darr_resize(arr, arr->len);
}

void darr_del(darr *arr)
{
	if (arr) {
		if (arr->data)
			free(arr->data);
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
	arr->data[arr->len] = el;
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
