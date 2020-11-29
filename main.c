#include <stdio.h>
#include "darr.h"
#include "hashmap.h"
#include "nutsh.h"
#include "cmd.h"
#include "environ.h"

int main(int argc, char* argv[])
{
	environ_init();
	// config();

	char *ps1 = "> ";

	char *line;
	char **args;
	int status;
	do
	{
		printf("%s", ps1);
		line = nsh_readline();
		args = nsh_splitline(line);
		status = nsh_execute(args);

		free(line);
		free(args);
		if (feof(stdin))
			break;
	} while (status);

	return 0;
}
