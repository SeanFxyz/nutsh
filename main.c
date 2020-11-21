#include <stdio.h>
#include "nutsh_string.h"

void process(nstr cmd)
{
	printf("%s\n", cmd.cs);
}

int main(int argc, char* argv[])
{
	nstr cmd = nstrNew();
	while (1) {
		nstrGetline(&cmd);
		process(cmd);
		nstrInit(&cmd);
	}

	return 0;
}
