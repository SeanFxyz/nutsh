#ifndef NSH_BUILTIN_H
#define NSH_BUILDIN_H

#include <stdio.h>
#include "nutsh.h"

char nsh_cd(char **args);
char nsh_help(char **args);
char nsh_exit(char **args);

// List of builtin commands
#define NSH_NUM_BUILTINS 3
char *builtin_str[] = {
	"cd",
	"help",
	"exit"
};

// Function pointers corresponding to command names in builtin_str
char (*builtin_func[]) (char **) = {
	&nsh_cd,
	&nsh_help,
	&nsh_exit
};

// Command implementations
char nsh_cd(char **args)
{
	if (args[1] == NULL) {
		fprintf(stderr, "nsh: expected argument to \"cd\"\n");
	} else {
		if (chdir(args[1]) != 0) {
			perror("nsh");
		}
	}
	return 1;
}

char nsh_help(char **args)
{
	printf("nutsh\n");
	printf("Builtin commands:\n");

	for (int i = 0; i < NSH_NUM_BUILTINS; i++)
		printf("\t%s\n", builtin_str[i]);

	return 1;
}

char nsh_exit(char **args)
{
	return 0;
}

// command execution
int nsh_execute(char **args)
{
	if (args[0] == NULL) {
		return 1;
	}

	for (int i=0; i < NSH_NUM_BUILTINS; i++) {
		if (strcmp(args[0], builtin_str[i]) == 0) {
			return (*builtin_func[i])(args);
		}
	}

	return nsh_launch(args);
}


#endif
