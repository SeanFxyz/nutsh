#ifndef _VARS_H
#define _VARS_H

#include "hashmap.h"
#include "dbg.h"

extern char** environ;

Hashmap *vars;

void var_declare(char *var);
void load_ext_vars(Hashmap *map);
void print_vars();

#define var_set(A, B) Hashmap_set(vars, (A), (B))
#define var_get(A) Hashmap_get(vars, (A))

void var_declare(char *var)
{
	size_t pos;
	char *key;
	char *value;
	for (pos = 0; var[pos] != '='; pos++)
		check(var[pos] != '\0',
				"nutsh: invalid variable declaration");

	key = malloc(pos);
	strncpy(key, var, pos);

	value = var + pos + 1;

	var_set(key, value);

	free(key);
}

void load_ext_vars(Hashmap *map)
{
	size_t i;

	for (i = 0; environ[i] != NULL; i++) {
		var_declare(environ[i]);
	}

}

void environ_init()
{
	vars = Hashmap_create(NULL, NULL);
	load_ext_vars(vars);
}

void print_vars()
{
	for (size_t i = 0; environ[i] != NULL; i++) {
		printf("%s\n", environ[i]);
	}
}

#endif
