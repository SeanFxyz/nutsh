#include <stdint.h>
#include "hashmap.h"
#include "dbg.h"

Hashmap *Hashmap_create(Hashmap_compare compare, Hashmap_hash hash)
{
	Hashmap *map = malloc(sizeof(Hashmap));
	check_mem(map);

	map->compare = compare == NULL ? default_compare : compare;
	map->hash = hash == NULL ? default_hash : hash;
	map->buckets = darr_create(sizeof(darr *), DEFAULT_BUCKETS);
	map->buckets->end = map->buckets->max;
	check_mem(map->buckets);

	return map;
}

void Hashmap_destroy(Hashmap *map)
{
	int i = 0;
	int j = 0;

	if (map) {
		if (map->buckets) {
			for (i = 0; i < darr_count(map->buckets); i++) {
				darr *bucket = darr_get(map->buckets, i);
				if (bucket) {
					for (j = 0; j < darr_count(bucket); j++) {
						free(darr_get(bucket, j));
					}
					darr_destroy(bucket);
				}
			}
			darr_destroy(map->buckets);
		}

		free(map);
	}
}

static inline HashmapNode *Hashmap_node_create(int hash, void *key, void *data)
{
	HashmapNode *node = calloc(1, sizeof(HashmapNode));
	check_mem(node);

	node->key = key;
	node->data = data;
	node->hash = hash;

	return node;
}

static inline darr *Hashmap_find_bucket(
		Hashmap *map,
		void *key,
		int create,
		uint32_t *hash_out)
{
	uint32_t hash = map->hash(key);
	int bucket_n = hash % DEFAULT_BUCKETS;
	check(bucket_n >= 0, "Invalid bucket found: %d", bucket_n);
	*hash_out = hash;

	darr *bucket = darr_get(map->bucket, bucket_n);

	if (!bucket && create) {
		// make a new bucket
		bucket = darr_create(sizeof(void *), DEFAULT_BUCKETS);
		check_mem(bucket);
		darr_set(map->buckets, bucket_n, bucket);
	}

	return bucket;
}

// TODO: Hashmap_set
