#include <stdio.h>
#include "nstr.h"
#include "darr.h"

extern char** environ;

void process(nstr cmd)
{
	printf("%s\n", cmd.cs);
}

int main(int argc, char* argv[])
{
	nstr str1 = nstr_new("");
	nstr_setcs(&str1, "Hello");
	nstr str2 = nstr_new("");
	nstr_setcs(&str2, "World");

	darr *my_arr = darr_new(sizeof(nstr), 5);
	darr_push(my_arr, &str1);
	darr_push(my_arr, &str2);

	// TODO: get strings back from array

	nstr cmd = nstr_new();
	while (1) {
		nstr_getline(&cmd);
		process(cmd);
		nstr_init(&cmd);
	}

	return 0;
}
