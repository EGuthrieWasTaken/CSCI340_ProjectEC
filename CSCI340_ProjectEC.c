// CSCI340_ProjectEC
// Ethan Guthrie
// 04/27/2020
// Creates a simple Unix shell.

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int runCommand(char *input, char *dir, char *path);

int main(int argc, char const *argv[]){
    // Setting default variables.
    char dir[256];
    char *input;
    size_t input_size = 256;
    input = (char *) malloc(input_size * sizeof(char));
    char path[256];
    getcwd(dir, sizeof(dir));
    sprintf(path, "/bin;");

    // Checking command-line arguments.
    if(argc == 1){
        // Running program in interactive mode.
        while(strcmp(input, "exit") != 0){
            // Printing prompt
            printf("ccsh> ");
            // Getting input and stripping newline.
            getline(&input, &input_size, stdin);
            strtok(input, "\n");
            // Running the command.
            if(runCommand(input, dir, path) == -1){
                char error_message[30] = "An error has occurred\n";
                write(STDERR_FILENO, error_message, strlen(error_message));
            }
            printf("%s\n", path);
        }
        exit(0);
    }else if(argc == 2){
        // Running program in batch mode.

        exit(0);
    }else{
        // Invalid number of arguments.
        printf("Usage:\t./CSCI340_ProjectEC.out [batch_file]\n");
        exit(0);
    }
}

int runCommand(char *input, char *dir, char *path){
    // Setting default variables.
    const char delim[2] = " ";
    char *token;

    // Setting token to first word in input.
    token = strtok(input, delim);

    // Checking if command is built-in or in path.
    if(strcmp(token, "exit") == 0){
        // Exit command specified.
        token = strtok(NULL, delim);
        if(token != NULL){
            return -1;
        }
        return 0;
    }else if(strcmp(token, "cd") == 0){
        // Changing directory (built-in).
        char *token2;
        // Getting directory.
        token = strtok(NULL, delim);
        if(token == NULL){
            return -1;
        }

        // Checking for extraneous arguments.
        token2 = strtok(NULL, delim);
        if(token2 != NULL){
            return -1;
        }

        // Changing directory.
        if(chdir(token) == -1){
            return -1;
        }

        // Updating dir.
        getcwd(dir, 256);
        return 0;
    }else if(strcmp(token, "path") == 0){
        // Updating path (built-in).
        DIR* path_dir;
        char buffer[256];
        // Iterating through arguments.
        token = strtok(NULL, delim);
        while(token != NULL){
            path_dir = opendir(token);
            if(!path_dir){
                return -1;
            }
            sprintf(buffer, "%s%s;", buffer, token);
            token = strtok(NULL, delim);
        }

        // Writing buffer to path.
        sprintf(path, "%s", buffer);
        return 0;
    }else{
        // Running command from path or current directory.
        // Checking to see if command is relative/absolute executable.
        if(!access(token, X_OK)){
            // Running relative/absolute executable.

        }else{
            // Running command from path.

        }
        return 0;
    }
}