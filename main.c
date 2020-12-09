#include <stdio.h>
#include <unistd.h>
#include "darr.h"
#include "hashmap.h"
#include "nutsh.h"
#include "cmd.h"
#include "environ.h"

int main(int argc, char* argv[])
{
	environ_init();

	char *ps1;
	if (getuid() == 0)
		ps1 = "🥜# ";
	else
		ps1 = "🥜$ ";

	char *line;
	char **args;
	int status;
	int i;
	do
	{
		printf("%s", ps1);
		line = nsh_readline();
		args = nsh_tokenize(line);
		status = nsh_execute(args);

		free(line);
		for (i = 0; args[i] != NULL; i++)
			free(args[i]);
		free(args[i]);

		if (feof(stdin)) {
			printf("\n");
			break;
		}
	} while (status);

	return 0;
}
