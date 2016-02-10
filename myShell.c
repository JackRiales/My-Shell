/*
=====================================================================================
    My Shell
    Jack Riales, CSC 322-501 Spring
    Takes in command line programs and runs them as child processes.
=====================================================================================
*/

// Checking os compatability.
// Only linux systems will be able to run this successfully.
#ifdef __linux__
    #define OSCOMPAT 1
#else
    #define OSCOMPAT 0
#endif

// Argument codes
#define ARG_HELP        "-h"
#define ARG_HELP_ALT    "--help"
#define ARG_VERB        "-v"
#define ARG_VER_ALT     "--verbose"

// Return codes
#define EXIT_SUCCESS    0
#define EXIT_FAILURE    -1

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>

// Verbosity flag, used to print more info.
int verbose = false;

/*
=====================================================================================
    Displays help info. Used in main when the argument is -h or --help
    \return EXIT_SUCCESS
=====================================================================================
*/
int help() {
    printf("\nMy Shell : by Jack Riales\n\nUsage:\t./myShell [-h/--help] [-v/--verbose] [prog] [args]\n\n-h\tHelp\tDisplays this help message.\n-v\tVerbose\tPrints out more information.\n\n");
    return EXIT_SUCCESS;
}

/*
=====================================================================================
    Executes an external program using the array of arguments provided.
    \param args The argument array. The 0 index is typically the name of the program,\
    while the rest tend to be arguments to that program.
    \return EXIT_SUCCESS on successful exit, EXIT_FAILURE on exception occurance.
=====================================================================================
*/
int execute_external(char *args[]) {
    // Create a program id and a status value for a new process.
    pid_t   pid = fork();
    int     status = 0;

    // Check if the new process wasn't able to be created
    if (pid < 0) {
        printf("Process unable to be created. PID : %d.\n", pid);
        return EXIT_FAILURE;
    }
    else if (pid == 0) {
        // Run the child process
        if (execvp(*args, args) < 0) {
            printf("Running %s failed. Are you sure the program exists?\nExiting.\n", args[0]);
            return EXIT_FAILURE;
        }
    }
    else {
        // Wait for the child process to finish up
        while (wait(&status) != pid) {}
    }
    return EXIT_SUCCESS;
}

/*
=====================================================================================
    Main driver
=====================================================================================
*/
int main(int argc, char *argv[]) {
    // Check if the OS is linux, and exit if not.
    if (!OSCOMPAT) {
        printf("Platform not supported. Please use a linux based OS to run this.");
        return EXIT_FAILURE;
    }

    // Check our arguments
    if (argc > 1) {
        char *run_args[16]; // the program and its args the user wants to run. Zero index will always be the program name.
        for (int i = 0; i < 16; ++i) run_args[i] = NULL; // initialize to null
        int c_run_args_index = 1; // keeps track of the latest run_args index
        for (int i = 1; i < argc; ++i) {
            char * arg = argv[i];

            // Use the help method
            if (!strcmp(arg, ARG_HELP) || !strcmp(arg, ARG_HELP_ALT)) {
                return help();
            }

            // Check for verbosity
            else if (!verbose && (!strcmp(arg, ARG_VERB) || !strcmp(arg, ARG_VER_ALT))) {
                verbose = true;
            }

            else {
                // Check for the program provided and its arguments.
                // The first occurance of a non-argument is the program the user is looking to run.
                if (run_args[0] == NULL) {
                    if (verbose) {
                        printf("Accepting %s as the program to run.\n", arg);
                    }
                    run_args[0] = arg;
                }
                else {
                    // All others are arguments to that program. Add these to the run args array.
                    if (verbose) {
                        printf("Adding %s as an argument to %s.\n", arg, run_args[0]);
                    }
                    run_args[c_run_args_index] = arg;
                    c_run_args_index++;
                }
            }
        }

        // Run the program the user wants
        return execute_external(run_args);

    } else {
        printf("\nNo arguments provided.");
        return help();
    }
}
