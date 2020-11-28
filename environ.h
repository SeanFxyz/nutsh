extern char** environ;

void print_vars() {
	for (size_t i = 0; environ[i] != NULL; i++) {
		printf("%s\n", environ[i]);
	}
}
