#include <stdint.h>
#include "hashmap.h"
#include "darr.h"
#include "dbg.h"

static int default_compare(void *a, void *b)
{
	return strcmp((char*)a, (char*)b);
}

// Bob Jenkins one_at_a_time hash
static uint32_t default_hash(void *a)
{
	size_t len = strlen((char*)a);
	char *key = (char*)a;

	uint32_t hash;
	uint32_t i;

	for (hash = i = 0; i < len; i++) {
		hash += key[i];
		hash += (hash << 10);
		hash ^= hash >> 6;
	}

	hash += hash << 3;
	hash ^= hash >> 11;
	hash += hash << 15;
	return hash;
}

Hashmap *Hashmap_create(Hashmap_compare compare, Hashmap_hash hash)
{
	Hashmap *map = malloc(sizeof(Hashmap));
	check_mem(map);

	map->compare = compare == NULL ? default_compare : compare;
	map->hash = hash == NULL ? default_hash : hash;
	map->buckets = darr_create(sizeof(darr *), DEFAULT_BUCKETS);
	map->buckets->len = map->buckets->cap;
	check_mem(map->buckets);

	return map;
}

void Hashmap_destroy(Hashmap *map)
{
	int i = 0;
	int j = 0;

	if (map) {
		if (map->buckets) {
			for (i = 0; i < map->buckets->len; i++) {
				darr *bucket = darr_get(map->buckets, i);
				if (bucket) {
					for (j = 0; j < bucket->len; j++) {
						free(darr_get(bucket, j));
					}
					darr_del(bucket);
				}
			}
			darr_del(map->buckets);
		}

		free(map);
	}
}

static inline HashmapNode *Hashmap_node_create(int hash, void *key, void *value)
{
	HashmapNode *node = calloc(1, sizeof(HashmapNode));
	check_mem(node);

	node->key = key;
	node->value = value;
	node->hash = hash;

	return node;
}

static inline darr *Hashmap_find_bucket(
		Hashmap *map,
		void *key,
		char create,
		uint32_t *hash_out)
{
	uint32_t hash = map->hash(key);
	int bucket_n = hash % DEFAULT_BUCKETS;
	check(bucket_n >= 0, "Invalid bucket found");
	*hash_out = hash;

	darr *bucket = darr_get(map->buckets, bucket_n);

	if (!bucket && create) {
		// make a new bucket
		bucket = darr_create(sizeof(void *), DEFAULT_BUCKETS);
		check_mem(bucket);
		darr_set(map->buckets, bucket_n, bucket);
	}

	return bucket;
}

int Hashmap_set(Hashmap *map, void *key, void *value)
{
	uint32_t hash = 0;
	darr *bucket = Hashmap_find_bucket(map, key, 1, &hash);
	check(bucket, "Error: can't create bucket");

	HashmapNode *node = Hashmap_node_create(hash, key, value);
	check_mem(node);

	darr_push(bucket, node);

	return 0;
}

static inline int Hashmap_get_node(
		Hashmap *map,
		uint32_t hash,
		darr *bucket,
		void *key)
{
	int i;
	for (i = 0; i < bucket->len; i++) {
		HashmapNode *node = darr_get(bucket, i);
		if (node->hash == hash && map->compare(node->key, key) == 0) {
			return i;
		}
	}

	return -1;
}

void *Hashmap_get(Hashmap *map, void *key)
{
	uint32_t hash = 0;
	darr *bucket = Hashmap_find_bucket(map, key, 0, &hash);
	if (!bucket) return NULL;
	int i = Hashmap_get_node(map, hash, bucket, key);
	if (i == -1) return NULL;
	HashmapNode *node = darr_get(bucket, i);
	check(node != NULL, "Failed to get node from bucket");

	return node->value;
}

int Hashmap_traverse(Hashmap *map, Hashmap_traverse_cb traverse_cb)
{
	int i = 0;
	int j = 0;
	int rc = 0;

	for (i = 0; i < map->buckets->len; i++) {
		darr *bucket = darr_get(map->buckets, i);
		if (bucket) {
			for (j = 0; j < bucket->len; j++) {
				HashmapNode *node = darr_get(bucket, j);
				rc = traverse_cb(node);
				if (rc != 0)
					return rc;
			}
		}
	}

	return 0;
}

void *Hashmap_delete(Hashmap *map, void *key)
{
	uint32_t hash = 0;
	darr *bucket = Hashmap_find_bucket(map, key, 0, &hash);
	if (!bucket) return NULL;

	int i = Hashmap_get_node(map, hash, bucket, key);
	if (i == -1) return NULL;

	HashmapNode *node = darr_get(bucket, i);
	void *value = node->value;
	free(node);

	HashmapNode *ending = darr_pop(bucket);

	if (ending != node) {
		// not the last bucket, swap it
		darr_set(bucket, i, ending);
	}

	return value;
}
