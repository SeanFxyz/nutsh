/*
 * Written by Sean M Fitzgerald
 * sean_m_fitzgerald@tutanota.com
 */

#ifndef NSH_BUILTIN_H
#define NSH_BUILTIN_H

#include <stdio.h>

char nsh_cd(char **args);
char nsh_pwd(char **args);
char nsh_help(char **args);
char nsh_exit(char **args);

// List of builtin commands
#define NSH_NUM_BUILTINS 4
char *builtin_str[] = {
	"cd",
	"pwd",
	"help",
	"exit"
};

// Function pointers corresponding to command names in builtin_str
char (*builtin_func[]) (char **) = {
	&nsh_cd,
	&nsh_pwd,
	&nsh_help,
	&nsh_exit
};

// Command implementations
char nsh_cd(char **args)
{
	if (args[1] == NULL) {
		printf("nsh: expected argument to \"cd\"\n");
	} else {
		if (chdir(args[1]) != 0) {
			perror("nsh");
		}
	}
	return 1;
}

char nsh_pwd(char **args)
{
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	printf("%s\n", cwd);
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

char (*nsh_get_builtin(char **args))(char **)
{
	int i;
	for (i = 0; i < NSH_NUM_BUILTINS; i++) {
		if (strcmp(args[0], builtin_str[i]) == 0) {
			return builtin_func[i];
		}
	}
	return NULL;
}

#endif
