 Implemented a UNIX shell in C that provides basic functionality to interact with a Linux system. 

The code basically does the following:

1. Prompts the user for an input command.
2. Parse the input line in order to determine whether to run another program or perform a built-in action.
3. Either perform the built-in, or
4. Fork itself and, in the child process, execute a program.
5. Wait for the program or action to exit.
6. Repeat.
