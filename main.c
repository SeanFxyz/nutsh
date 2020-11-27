#include <stdio.h>
#include "nutsh.h"
#include "cmd.h"

extern char** environ;

char ps1[] = ">";
char path[] = "/usr/bin:/bin";

int main(int argc, char* argv[])
{
	char *line;
	char **args;
	int status;
	do {
		printf("%s", ps1);
		line = nsh_readline();
		args = nsh_splitline(line);
		status = nsh_execute(args);

		free(line);
		free(args);
	} while (status);

	return 0;
}
