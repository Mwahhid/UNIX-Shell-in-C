#include "cwd.h" // for using cwd()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

extern char **environ;

#define HIST_SIZE 50
#define CMD_SIZE 512
#define MAX_ARGS 512

int main(int argc, char **argv) {
  printf("Welcome to MyShell!\nPath: ");
  char *envPtr = getenv("PATH");
  char pathString[MAX_ARGS];
  char input[CMD_SIZE];
  char cwd[MAX_ARGS];
  char *args[MAX_ARGS];
  char temp[CMD_SIZE];
  char *paths[MAX_ARGS];
  char fileName[CMD_SIZE];
  char cmdHist[5][CMD_SIZE];

  int end = 0;   /* write index */
  int start = 0; /* read index */

  strcpy(pathString, envPtr);
  const char symbol[2] = ":";
  char *token;
  int x = 0;
  int k = -1;

  token = strtok(pathString, symbol);

  while (token != NULL) {
    paths[x] = token;
    x++;
    printf("\t%s\n", token);
    token = strtok(NULL, symbol);
  }

  paths[x] = NULL;

  while (1) {
    printf("$ ");
    fgets(input, CMD_SIZE, stdin);

    if (strncmp(input, "history", 7) != 0 && strncmp(input, "!", 1) != 0) {
      strcpy(cmdHist[end++], input);
      end %= HIST_SIZE;
      if (k != HIST_SIZE - 1) // k = # of commands in history
        k++;
      if (end > start && k == HIST_SIZE - 1) {
        start = end;
      }
    } else if (strncmp(input, "!", 1) == 0) {
      input[0] = ' ';
      int cmNum = atoi(input);
      int a = k;
      int index = start;

      while (a >= 0) {
        if (a == cmNum) {
          strcpy(input, cmdHist[index]);
          break;
        }
        index++;
        index %= HIST_SIZE;
        a--;
      }
    }

    strcpy(temp, input);         // copying the full command to temp variable
    temp[strlen(input) - 1] = 0; //    removing the \n character from the end

    if (strncmp(input, "exit", 4) == 0) {
      break;
    } else if (strncmp(input, "history", 7) == 0) {
      int a = k;
      int index = start;

      while (a >= 0) {
        printf("  %d: %s", a, cmdHist[index]);
        index++;
        index %= HIST_SIZE;
        a--;
      }

    } else if (strcmp(input, "\n") == 0) {
      printf("no command specified\n");
    } else if (strncmp(input, "pwd", 3) == 0) {
      if (getcwd(cwd, MAX_ARGS) == NULL)
        perror("pwd");
      else
        printf("%s\n", cwd);
    } else if (strncmp(input, "cd", 2) == 0) {
      token = strtok(temp, " ");
      token = strtok(NULL, " ");
      if (chdir(token) != 0) {
        perror("cd");
      }
    } else if (strncmp(input, "./", 2) == 0) {
      token = strtok(temp, " ");
      int i = 0;

      while (token != NULL) {
        args[i] = token;
        i++;
        token = strtok(NULL, " ");
      }

      args[i] = NULL;
      pid_t pid = fork();
      if (pid == -1) {
        // handle error
        perror("fork");
        exit(pid);
      } else if (pid == 0) {
        // this is the child process...
        // exec another program, or do child tasks...
        if (execve(args[0], args, environ) == -1) {
          perror("execve");
          exit(pid);
        }
      } else {
        // this is the parent process...
        // do parent tasks, or...
        wait(NULL); // wait for a child to finish.
      }
    } else {
      token = strtok(temp, " ");
      strcpy(fileName, token);
      int j = 0;

      for (j = 0; j < x; j++) { // iterate over all directories of PATH
        strcpy(pathString, paths[j]);
        pathString[strlen(paths[j])] = '/';
        pathString[strlen(paths[j]) + 1] = 0;
        strcat(pathString, fileName);        // form a full path of file
        if (access(pathString, X_OK) == 0) { // if file found and executable
          args[0] = pathString;
          token = strtok(NULL, " ");
          int i = 1;

          while (token != NULL) {
            args[i] = token;
            i++;
            token = strtok(NULL, " ");
          }

          args[i] = NULL;
          pid_t pid = fork();
          if (pid == -1) {
            // handle error
            perror("fork");
            exit(pid);
          } else if (pid == 0) {
            // this is the child process...
            // exec another program, or do child tasks...
            if (execve(args[0], args, environ) == -1) {
              perror("execve");
              exit(pid);
            }
          } else {
            // this is the parent process...
            // do parent tasks, or...
            wait(NULL); // wait for a child to finish.
          }

          break; // break out of the for loop
        }
      }

      if (j == x)
        printf("%s: command not found\n", fileName);
    }
  }

  return 0;
}
