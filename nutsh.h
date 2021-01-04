/*
 * Written by Sean M Fitzgerald
 * sean_m_fitzgerald@tutanota.com
 */

#ifndef NSH_H
#define NSH_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wait.h>
#include <unistd.h>
#include <errno.h>

#include "environ.h"
#include "cmd.h"

#define NSH_READLINE_BUFSIZE 1024

#define NSH_TOK_BUFSIZE 64
#define NSH_TOK_DELIM " \t\r\n\a"

static char *NSH_SYM_TOK[] = {">", "<", "||", "&&", "|", "&"};
#define NSH_SYM_TOK_N 6

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

char ch_in_arr(char ch, char *arr, char n)
{
	for (int i = 0; i < n; i++)
		if (arr[i] == ch)
			return 1;
	return 0;
}

void nsh_toksplit(char *token, darr *tokens)
{
	char *subtok_copy;
	char *split = NULL;
	size_t subtok_len;
	size_t sym_len;
	int i;

	for (i = 0; i < NSH_SYM_TOK_N; i++) {
		sym_len = strlen(NSH_SYM_TOK[i]);
		split = strstr(token, NSH_SYM_TOK[i]);
		if (split) {
			subtok_len = split - token;
			subtok_copy = malloc(subtok_len);
			strncpy(subtok_copy, token, subtok_len);
			darr_push(tokens, subtok_copy);

			subtok_copy = malloc(sym_len);
			strncpy(subtok_copy, NSH_SYM_TOK[i], subtok_len);
			darr_push(tokens, subtok_copy);

			return nsh_toksplit(split + sym_len, tokens);
		}
	}
	subtok_copy = malloc(strlen(token));
	strcpy(subtok_copy, token);
	darr_push(tokens, subtok_copy);
}

char **nsh_tokenize(char *line)
{
	darr *tokens = darr_create(sizeof(char*), 64);
	char **tokens_p;
	char *token;
	char *tok_copy;
	size_t tok_len;
	int i;

	token = strtok(line, NSH_TOK_DELIM);
	while (token != NULL) {
		tok_len = strlen(token);

		tok_copy = malloc(tok_len);
		check_mem(tok_copy);
		strcpy(tok_copy, token);

		// while space-delimited token ends in backslash, merge tokens
		while (tok_copy[tok_len - 1] == '\\') {
			tok_copy[tok_len - 1] = ' ';
			token = strtok(NULL, NSH_TOK_DELIM);
			tok_len += strlen(token);

			tok_copy = realloc(tok_copy, tok_len);
			check_mem(tok_copy);
			strcat(tok_copy, token);
		}

		// break up space-delimited token based on symbol token characters
		// push split-up tokens to darr
		nsh_toksplit(tok_copy, tokens);

		token = strtok(NULL, NSH_TOK_DELIM);
	}

	// push terminating null pointer and shrink darr to data size
	darr_push(tokens, NULL);
	darr_contract_full(tokens);

	tokens_p = (char**)tokens->data;
	free(tokens);

	i = 0;
	while (tokens_p[i] != NULL) {
		printf("%s\n", tokens_p[i]);
		i++;
	}

	return tokens_p;
}

static char **nsh_pipesplit_i = NULL;
char **nsh_pipesplit(char **tokens)
{
	int i;
	char **tok;

	if (tokens != NULL) {
		nsh_pipesplit_i = NULL;
		tok = tokens;
	} else if (nsh_pipesplit_i != NULL) {
		tok = nsh_pipesplit_i;
	} else {
		return NULL;
	}

	i = 0;
	for (i = 0;	tok[i] != NULL && strcmp(tok[i], "|") != 0;	i++);
	if (tok[i]) {
		free(tok[i]);
		tok[i] = NULL;
		nsh_pipesplit_i = &tok[i+1];
	} else {
		nsh_pipesplit_i = NULL;
	}

	return tok;
}

int nsh_runcmd(char **args, int infd)
{
	int status;
	pid_t pid;
	pid_t wpid;
	int pipefd[2];
	int outfd = STDOUT_FILENO;
	char **next_args = nsh_pipesplit(NULL);
	char (*builtin_func)(char **);

	if (next_args) {
		// we will pipe into the next command
		if (pipe(pipefd) < 0) {
			perror("nsh");
			exit(errno);
		}
		outfd = pipefd[1];
	}

	builtin_func = nsh_get_builtin(args);
	if (builtin_func) {
		return (*builtin_func)(args);
	}

	pid = fork();
	if (pid < 0) {
		perror("nsh");
		exit(errno);
	}

	if (pid == 0) {
		// child process
		// read from infd
		dup2(infd, STDIN_FILENO);

		// if we made another pipe
		if (outfd != 1) {
			// close read end
			close(pipefd[0]);
			// set up to write to new pipe
			dup2(pipefd[1], STDOUT_FILENO);
		}

		if (execvp(args[0], args) < 0) {
			perror("nsh");
			exit(errno);
		}

		exit(0);

	} else {
		// parent process
		if (next_args != NULL && outfd != STDOUT_FILENO) {
			// close write end of pipe
			close(pipefd[1]);
			// pipe to next command in pipeline
			return nsh_runcmd(next_args, pipefd[0]);
		}

		do {
			wpid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));

		return 1;
	}
}

int nsh_execute(char **args)
{
	if (args[0] == NULL)
		return 1;
	args = nsh_pipesplit(args);
	return nsh_runcmd(args, 0);
}

#endif
