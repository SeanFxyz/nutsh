/*
 * Written by Sean M Fitzgerald
 * sean_m_fitzgerald@tutanota.com
 */

#ifndef NSH_BUILTIN_H
#define NSH_BUILTIN_H

#include <stdio.h>

char cmd_cd(char **args);
char cmd_pwd(char **args);
char cmd_help(char **args);
char cmd_exit(char **args);

// List of builtin commands
#define NSH_NUM_BUILTINS 4
char *builtin_str[] = {
    "cd",
    "pwd",
    "help",
    "exit"
};

// Array of function pointers corresponding to command names in builtin_str
// This is an array of (deep breath):
// "pointers to functions that take a char** as an argument and return a char"
char (*builtin_func[]) (char **) = {
    &cmd_cd,
    &cmd_pwd,
    &cmd_help,
    &cmd_exit
};

// Command implementations

// cd: change current directory
char cmd_cd(char **args)
{
    if (args[1]) {
        // if first argument after "cd" is not null, change to that directory
        if (chdir(args[1]) != 0) {
            // if changing directories failed, error
            perror("nsh");
        }
    } else {
        // If no directory argument is given, tell user to give us one.
        // TODO: Should chdir to the home directory
        printf("nsh: expected argument to \"cd\"\n");
    }
    // Return good status
    return 1;
}

// pwd: print working directory
char cmd_pwd(char **args)
{
    // Create a buffer to store the current working directory.
    // TODO: resize buffer and try again if we mess up.
    char cwd[1024];

    // Get current working directory and store in buffer.
    getcwd(cwd, sizeof(cwd));

    // Print buffer contents to the screen.
    printf("%s\n", cwd);

    // Return good status
    return 1;
}

// help: show list of builtin commands
char cmd_help(char **args)
{
    printf("nutsh\n");
    printf("Builtin commands:\n");

    for (int i = 0; i < NSH_NUM_BUILTINS; i++)
        printf("\t%s\n", builtin_str[i]);

    return 1;
}

// exit: exit the shell
char cmd_exit(char **args)
{
    // return "bad" status, telling the main loop to exit
    return 0;
}

// This function's signature looks ugly and confusing, because
// function pointer syntax is ugly and confusing.
// It takes a char** (array of strings) as an argument and returns a:
// "pointer to a function that takes a char** as an argument and returns a char"
char (*nsh_get_builtin(char **args))(char **)
{
    int i;
    for (i = 0; i < NSH_NUM_BUILTINS; i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return builtin_func[i];
        }
    }
    return NULL;
}

#endif
