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

#include "cmd.h"

#define NSH_READLINE_BUFSIZE 1024

#define NSH_TOK_BUFSIZE 64
#define NSH_TOK_DELIM " \t\r\n\a"

// Reads a line of text from standard input, returning
// a pointer to the resulting string.
// TODO: Make more efficient.
char *nsh_readline()
{
    int bufsize = NSH_READLINE_BUFSIZE; // Default buffer size
    int pos = 0;                        // Current number of characters read
    int c;                              // For storing the next character value
    char *buffer = malloc(sizeof(char) * bufsize); // Allocate buffer

    // TODO: use mem_check macro
    if (!buffer) {
        fprintf(stderr, "nsh: allocation error\n");
        exit(1);
    }

    // Read characters into buffer.
    while ((c = getchar()) != '\n' && c != EOF) {
        buffer[pos] = c;
        pos++;

        if (pos >= bufsize) {
            // If we have filled our buffer, increase the size and reallocate

            // increase desired buffer size
            bufsize += NSH_READLINE_BUFSIZE;

            // Request resized memory block from the system.
            //  If the system isn't able to simply grow our memory block, this
            //  will be an O(n) operation where n is the old bufsize,
            //  but hopefully resizing won't happen too often.
            buffer = realloc(buffer, bufsize);

            // TODO: use mem_check macro
            if (!buffer) {
                fprintf(stderr, "nsh: allocation error\n");
                exit(1);
            }
        }
    }

    // Make the buffer a proper C-string by adding a null character on the end
    buffer[pos] = '\0';

    return buffer;
}

char **nsh_tokenize(char *line)
{
    darr *tokens = darr_create(sizeof(char*), 64);
    char **tokens_p;
    size_t tok_len;
    char *token;
    char *tok_copy;

    token = strtok(line, NSH_TOK_DELIM);
    while (token != NULL) {
        tok_len = strlen(token);

        tok_copy = malloc(tok_len);
        check_mem(tok_copy);
        strcpy(tok_copy, token);

        while (tok_copy[tok_len - 1] == '\\') {
            tok_copy[tok_len - 1] = ' ';
            token = strtok(NULL, NSH_TOK_DELIM);
            tok_len += strlen(token);

            tok_copy = realloc(tok_copy, tok_len);
            check_mem(tok_copy);
            strcat(tok_copy, token);
        }

        darr_push(tokens, tok_copy);

        token = strtok(NULL, NSH_TOK_DELIM);
    }

    // push terminating null pointer and shrink darr to data size
    darr_push(tokens, NULL);
    darr_contract_full(tokens);

    tokens_p = (char**)tokens->data;
    free(tokens);

    return tokens_p;
}

// nsh_pipesplit works sort of like strtok. It will return a pointer within
// the same array of strings, indicating the start of the next sequence, as
// well as setting the pipe token (a char*) at the end of the sequence to
// NULL, which is used as a sentinel by nsh_runcmd

// pointer to the next sequence in the most recently split token list
// This is what nsh_pipesplit will return if it is passed NULL for its argument
// If it is passwd a non-NULL pointer
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
    // Check that we have arguments to process.
    if (args[0] == NULL)
        return 1;

    // Get a pointer to the next pipe-separated sequence of tokens in args.
    // nsh_pipesplit works sort of like strtok. It will return a pointer within
    // the same array of strings, indicating the start of the next sequence, as
    // well as setting the pipe token (a char*) at the end of the sequence to
    // NULL, which is used as a sentinel by nsh_runcmd
    args = nsh_pipesplit(args);

    // Call recursive nsh_runcmd function to execute this first
    // sequence as well as all subsequent sequences.
    return nsh_runcmd(args, 0);
}

#endif
