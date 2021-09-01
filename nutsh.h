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
// TODO: Make more efficient. One character at a time is slow. Maybe use scanf?
char *nsh_readline()
{
    int bufsize = NSH_READLINE_BUFSIZE; // Default buffer size
    int pos = 0;                        // Current number of characters read
    int c;                              // For storing the next character value
    char *buffer = malloc(sizeof(char) * bufsize); // Allocate buffer

    // Check allocation (check_mem macro defined in dbg.h)
    check_mem(buffer);

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

            // Check reallocation
            check_mem(buffer);
        }
    }

    // Make the buffer a proper C-string by adding a null character on the end
    buffer[pos] = '\0';

    return buffer;
}

// Expects a line of text, breaks it up into tokens, and returns an array of
// token strings.
// Tokens are separated by unescaped spaces.
//
// Originally, this simply used the standard library's 'strtok' to break the
// line into tokens, but this meant that tokens could not escape spaces.
//
// Now, it uses strtok break off the first char sequence with no spaces, then
// copies the sequence to a separate buffer at tok_copy. If the sequence ends
// in a backslash, this indicates the space between this sequence and the next
// was meant to be escaped, so the next token will be merged with the current
// token, and this is repeated until the current sequence no longer ends with
// a backslash. The tok_copy pointer's current address is then pushed onto a
// dynamic array. This process in repeated until the line has been processed
// and each token has been pushed into the dynamic array. The dynamic array is
// then terminated with a NULL sentinel value and contracted to free unused
// memory, and a normal char** tokens_p is assigned to the dynamic array's data
// addres and returned.
char **nsh_tokenize(char *line)
{
    // Dynamic array to store tokens.
    darr *tokens = darr_create(sizeof(char*), 64);

    char **tokens_p; // Will be set to address of final char** and returned.
    size_t tok_len;  // To keep track of the length of the current token.
    char *token;     // Keeps track of the current token
                     //   (this is an address within the original 'line' string)
    char *tok_copy;  // Copy of 'token' that will be added to 'tokens'

    // Get a pointer to the first space-delimited sequence within line
    token = strtok(line, NSH_TOK_DELIM);

    // While we have a sequence to process...
    while (token != NULL) {

        // Get the token length
        tok_len = strlen(token);

        // Allocate a buffer to make a copy of the sequence
        tok_copy = malloc(tok_len);
        check_mem(tok_copy); // check allocation

        // Copy sequence to the new buffer.
        strcpy(tok_copy, token);

        // While the copy's last character is a backslash...
        while (tok_copy[tok_len - 1] == '\\') {
            // Replace the backslash with a space
            tok_copy[tok_len - 1] = ' ';

            // Get the next space-delimited sequence
            // (NULL for the first argument means strtok will get the next tok in
            // the last string given)
            token = strtok(NULL, NSH_TOK_DELIM);

            // Add the new 'token' sequence's length the the total
            tok_len += strlen(token);

            // Grow the buffer at 'tok_copy' to accomodate the new sequence
            tok_copy = realloc(tok_copy, tok_len);
            check_mem(tok_copy); // check allocation

            // Add the new sequence to the end of tok_copy
            strcat(tok_copy, token);
        }

        // Add tok_copy's current value to the end of the dynamic array.
        darr_push(tokens, tok_copy);

        // Get the next space-delimited sequence
        // (NULL for the first argument means strtok will get the next tok in
        // the last string given)
        token = strtok(NULL, NSH_TOK_DELIM);
    }

    // push terminating null pointer and shrink darr to data size
    darr_push(tokens, NULL);
    darr_contract_full(tokens);

    tokens_p = (char**)tokens->data;
    free(tokens);

    return tokens_p;
}

// nsh_pipesplit works sort of like strtok, but rather than strings, it works
// on arrays of strings. It will return a pointer within the same array of
// strings, indicating the start of the next sequence, as well as setting the
// pipe token (a char*) at the end of the sequence to NULL, which is used as a
// sentinel by nsh_runcmd

// pointer to the next sequence in the most recently split token list
// This is what nsh_pipesplit will return if it is passed NULL for its argument
// If nsh_pipesplit is passed a non-NULL pointer, nsh_pipesplit_i will be set
// to point to the sequence after the first pipe
static char **nsh_pipesplit_i = NULL;
char **nsh_pipesplit(char **tokens)
{
    int i;      // index for the for-loop below
    char **tok; // the pointer we will return, unless we are at the end of the
                // tokens and return NULL

    if (tokens != NULL) {
        // If we are given a new array of tokens to split:

        // Reset nsh_pipesplit_i to NULL (it will be later set to the start of
        // the next sequence, after the first pipe
        nsh_pipesplit_i = NULL;

        // Set the return value to the beginning of the new array.
        tok = tokens;

    } else if (nsh_pipesplit_i != NULL) {
        // If we don't have a new array to process, AND nsh_pipesplit_i is set
        // from working on a previous array:

        // Set return value to nsh_pipesplit_i, which should point to the next
        // pipe-delimited sequence of tokens.
        tok = nsh_pipesplit_i;

    } else {
        // If none of the above, we've either reached the end of the current
        // token sequence or weren't working on one to begin with, so return
        // NULL
        return NULL;
    }

    // Now it's time to adjust the nsh_pipesplit_i pointer to point to the
    // token at the start of the next pipe-delimited sequence.

    // Initialize i
    i = 0;

    // Yes, this is a for-loop with no body. This causes i to be incremented
    // until tok[i] is either NULL or a string equivalent to "|", a pipe token
    for (i = 0;	tok[i] != NULL && strcmp(tok[i], "|") != 0;	i++);

    if (tok[i]) {
        // tok[i] is not NULL, which because of the for-loop above means it
        // must point to a string equivalent to "|"

        // Free this string and set the pointer at tok[i] to NULL
        free(tok[i]);
        tok[i] = NULL;

        // Set nsh_pipesplit_i to the start of the next sequence, one byte
        // past the discovered pipe token
        nsh_pipesplit_i = &tok[i+1];
    } else {
        // tok[i] is NULL, so we've reached the end of this token sequence.
        nsh_pipesplit_i = NULL;
    }

    // Return the pointer
    return tok;
}

int nsh_runcmd(char **args, int infd)
{
    int status;    // status to return to the main loop
    pid_t pid;     // stores process id of forked process
    int pipefd[2]; // stores the file descriptors for the two ends of a pipe
    int outfd = STDOUT_FILENO;     // the file descriptor to use for output,
                                   //   defaulted to standard output
    char (*builtin_func)(char **); // function pointer to reference builtins

    // Grab the next pipe-delimited sequence and store in next_args
    char **next_args = nsh_pipesplit(NULL);

    // If we actually have another pipe-delimited sequence, we want to pipe
    // the output of this command into the output of the next one, so we will
    // create a pipe and pass its input file descriptor for infd when we
    // recurse.
    if (next_args) {
        // Create pipe and store its input and output file descriptors in
        // the pipefd array
        if (pipe(pipefd) < 0) {
            // Error out if the pipe wasn't created successfully.
            perror("nsh");
            exit(errno);
        }

        // Change the output file descriptor used for the current command
        // to the pipe's input
        outfd = pipefd[1];
    }

    // Look for a builtin command matching the tokens we've been given.
    // (This sets builtin_func to the appropriate function's address)
    // TODO: Instead of returning, let the builtin command work with pipes.
    builtin_func = nsh_get_builtin(args);
    if (builtin_func) {
        // If nsh_get_builtin gave us a non-NULL function pointer, call the
        // function at the address with args as the argument.
        return (*builtin_func)(args);
    }

    // Fork the current process, storing the process ID of the child process.
    pid = fork();
    if (pid < 0) {
        // If the pid is negative, fork was unsuccessful
        perror("nsh");
        exit(errno);
    }

    if (pid == 0) {
        // If our PID is zero, we are the child process

        // If this command is being piped into, we want to get our input from
        // the previous process so duplicate the provided input file descriptor
        // (infd) to STDIN_FILENO, overwriting the standard input file
        // descriptor.
        dup2(infd, STDIN_FILENO);

        if (outfd != 1) {
            // If our outfd isn't equal to the default standard output file
            // descriptor:

            // close the read end
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
            waitpid(pid, &status, WUNTRACED);
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
