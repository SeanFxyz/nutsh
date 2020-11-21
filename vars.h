#include "nutsh_string.h"

typedef struct env_var {
	char *name;
	char *value;
} envar ;



char *setVar(char *name)
{
