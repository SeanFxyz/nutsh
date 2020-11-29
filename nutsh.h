#ifndef NSH_H
#define NSH_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wait.h>
#include <unistd.h>

#include "environ.h"

#define NSH_READLINE_BUFSIZE 1024

#define NSH_TOK_BUFSIZE 64
#define NSH_TOK_DELIM " \t\r\n\a"

char *nsh_readline()
{
	int bufsize = NSH_READLINE_BUFSIZE;
	int pos = 0;
	char *buffer = malloc(sizeof(char) * bufsize);
	int c;

	if (!buffer) {
		fprintf(stderr, "nsh: allocation error\n");
		exit(1);
	}

	while ((c = getchar()) != '\n' && c != EOF) {
		buffer[pos] = c;
		pos++;

		if (pos >= bufsize) {
			bufsize += NSH_READLINE_BUFSIZE;
			buffer = realloc(buffer, bufsize);
			if (!buffer) {
				fprintf(stderr, "nsh: allocation error\n");
				exit(1);
			}
		}
	}

	buffer[pos] = '\0';
	return buffer;
}

char **nsh_splitline(char *line)
{
	int bufsize = NSH_TOK_BUFSIZE;
	int pos = 0;
	char **tokens = malloc(bufsize * sizeof(char*));
	char *token;

	int i;
	int tok_end;
	char *next_tok;
	int new_len;

	if (!tokens) {
		fprintf(stderr, "nsh: allocation error\n");
		exit(1);
	}

	token = strtok(line, NSH_TOK_DELIM);
	while (token != NULL) {

		tok_end = strlen(token) - 1;
		while (token[tok_end] == '\\') {
			// append next token
			next_tok = strtok(NULL, NSH_TOK_DELIM);
			new_len = tok_end + strlen(next_tok);
			char *new_tok = malloc(new_len);
			check_mem(new_tok);
			strcpy(token + tok_end, next_tok);
			tok_end = new_len;
		}

		tokens[pos] = token;
		pos++;

		if (pos >= bufsize) {
			bufsize += NSH_TOK_BUFSIZE;
			tokens = realloc(tokens, bufsize * sizeof(char*));
			if (!tokens) {
				fprintf(stderr, "nsh: allocation error\n");
				exit(1);
			}
		}

		token = strtok(NULL, NSH_TOK_DELIM);
	}
	tokens[pos] = NULL;
	return tokens;
}

int nsh_launch(char **args)
{
	pid_t pid;
	pid_t wpid;

	int status;

	pid = fork();
	if (pid == 0) {
		// child process
		if (execvp(args[0], args) == -1)
			perror("nsh");
		exit(1);
	} else if (pid < 0) {
		// fork failed
		perror("nsh");
	} else {
		// parent process
		do {
			// wait for child process to complete
			wpid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return 1;
}

#endif
