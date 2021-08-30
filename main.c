/*
 * Written by Sean M Fitzgerald
 * sean_m_fitzgerald@tutanota.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "darr.h"
#include "nutsh.h"
#include "cmd.h"

int main(int argc, char* argv[])
{
    // Shells generally use the PS1 environment variable as the prompt,
    // that bit of text that appears before each command.
    // Although nutsh doesn't yet allow the user to get or set environment
    // variables, and doesn't have a config file to initialize it, we will
    // use PS1 to hold our standard prompt.
    if (getuid() == 0)
        // if nutsh is run as the root user, use a '#' in the prompt
        setenv("PS1", "🥜# ", 1);
    else
        // otherwise, use a '$'
        setenv("PS1", "🥜$ ", 1);


    char *line;  // The current line of input to be processed.
    char **toks; // The current line, broken into an array of token strings.
    int status;  // If zero, the shell should terminate.
    int i;       // index for our for-loop below

    // Shell main loop, keeps running as long as our status is goot.
    do {
        printf("%s", getenv("PS1")); // Put a prompt at the start of the line
        line = nsh_readline();       // Read a line of input
        toks = nsh_tokenize(toks);   // Break the line of input into tokens
        status = nsh_execute(toks);  // Execute command

        // Memory cleanup
        free(line);
        for (i = 0; toks[i] != NULL; i++)
            free(toks[i]);
        free(toks[i]);

        // Terminate shell if EOF is reached
        if (feof(stdin)) {
            printf("\n");
            break;
        }

    } while (status); // End loop if status is not 0

    // Exit normally
    return 0;
}
