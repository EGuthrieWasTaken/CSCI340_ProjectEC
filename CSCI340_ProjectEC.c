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
int runParalellCommands(char *input, char *dir, char *path);

int main(int argc, char const *argv[]){
    // Setting default variables.
    char dir[256];
    char *input;
    size_t input_size = 256;
    input = (char *) malloc(input_size * sizeof(char));
    char path[256];
    getcwd(dir, sizeof(dir));
    sprintf(path, "/bin;");
    int out;

    // Checking command-line arguments.
    if(argc == 1){
        // Running program in interactive mode.
        while(strcmp(input, "exit") != 0){
            // Printing prompt
            printf("ccsh> ");
            // Getting input and stripping newline.
            getline(&input, &input_size, stdin);
            strtok(input, "\n");
            // Checking if parallel command.
            if(strstr(input, "&") != NULL){
                // Running parallel commands.
                out = runParalellCommands(input, dir, path);
                if(out == 1){
                    exit(0);
                }
            }else{
                // Running single command.
                out = runCommand(input, dir, path);
                if(out == 1){
                    exit(0);
                }
            }
        }
        printf("howdy\n");
    }else if(argc == 2){
        FILE *fp;
        char *line;
        size_t len = 0;
        ssize_t read;
        // Opening batch file.
        fp = fopen(argv[1], "r");
        if(fp == NULL){
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(-1);
        }
        // Reading file.
        while((read = getline(&line, &len, fp)) != -1){
            strtok(line, "\n");
            // Checking if parallel command.
            if(strstr(line, "&") != NULL){
                // Running parallel commands.
                out = runParalellCommands(line, dir, path);
                if(out == 1){
                    exit(0);
                }
            }else{
                // Running single command.
                out = runCommand(line, dir, path);
                if(out == 1){
                    exit(0);
                }
            }
        }

        exit(0);
    }else{
        // Invalid number of arguments.
        printf("Usage:\t./CSCI340_ProjectEC.out [batch_file]\n");
        exit(0);
    }
}

int runCommand(char *input, char *dir, char *path){
    // Setting default variables.
    const char delim[] = " ";
    char *token;

    // Getting command.
    char command[64];
    char inputSaved[256];
    sprintf(inputSaved, "%s", input);
    token = strtok(input, delim);
    sprintf(command, "%s", token);

    // Checking if command is built-in or in path.
    if(strcmp(command, "exit") == 0){
        // Exit command specified.
        token = strtok(NULL, delim);
        if(token != NULL){
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
        return 1;
    }else if(strcmp(command, "cd") == 0){
        // Changing directory (built-in).
        char *token2;
        // Getting directory.
        token = strtok(NULL, delim);
        if(token == NULL){
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
        }

        // Checking for extraneous arguments.
        token2 = strtok(NULL, delim);
        if(token2 != NULL){
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
        }

        // Changing directory.
        if(chdir(token) == -1){
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
        }

        // Updating dir.
        getcwd(dir, 256);
        return 0;
    }else if(strcmp(command, "path") == 0){
        // Updating path (built-in).
        DIR* path_dir;
        char buffer[256] = "";
        // Iterating through arguments.
        token = strtok(NULL, delim);
        while(token != NULL){
            path_dir = opendir(token);
            if(!path_dir){
                   char error_message[30] = "An error has occurred\n";
                    write(STDERR_FILENO, error_message, strlen(error_message));
            }
            sprintf(buffer, "%s%s;", buffer, token);
            token = strtok(NULL, delim);
        }

        // Writing buffer to path.
        sprintf(path, "%s", buffer);
        return 0;
    }else{
        pid_t pid = fork();
        if(pid == 0){
            // Running command from path or relative path.
            if(access(command, X_OK) == 0){
                // Running command from relative path.
                // Getting args.
                char buffer[256];
                token = strtok(NULL, delim);
                int counter = 0;
                if(token != NULL){
                    counter++;
                    sprintf(buffer, "%s", token);
                    token = strtok(NULL, delim);
                }
                while(token != NULL){
                    counter++;
                    sprintf(buffer, "%s %s", buffer, token);
                    token = strtok(NULL, delim);
                }

                char *args[counter + 2];
                args[0] = command;
                token = strtok(buffer, delim);
                for(int i = 1; i < counter + 2; i++){
                    args[i] = token;
                    token = strtok(NULL, delim);
                }
                args[counter + 1] = NULL;

                execv(args[0], args);
                return 0;
            }else{
                // Running command from path.
                const char path_delim[] = ";";
                char buffer[256];
                // Searching for command in path.
                token = strtok(path, path_delim);
                while(token != NULL){
                    sprintf(buffer, "%s/%s", token, command);
                    if(access(buffer, X_OK) == 0){
                        // Command found. Executing.
                        sprintf(command, "%s", buffer);
                        token = strtok(inputSaved, delim);
                        token = strtok(NULL, delim);
                        int counter = 0;
                        if(token != NULL){
                            counter++;
                            sprintf(buffer, "%s", token);
                            token = strtok(NULL, delim);
                        }
                        while(token != NULL){
                            counter++;
                            sprintf(buffer, "%s %s", buffer, token);
                            token = strtok(NULL, delim);
                        }

                        char *args[counter + 2];
                        args[0] = command;
                        token = strtok(buffer, delim);
                        for(int i = 1; i < counter + 2; i++){
                            args[i] = token;
                            token = strtok(NULL, delim);
                        }
                        args[counter + 1] = NULL;

                        execv(args[0], args);
                        return 0;
                    }
                    token = strtok(NULL, path_delim);
                }
                char error_message[30] = "An error has occurred\n";
                write(STDERR_FILENO, error_message, strlen(error_message));
            }
        }else{
            waitpid(pid, 0, 0);
        }
    }
}

int runParalellCommands(char *input, char *dir, char *path){
    // Setting default variables.
    int exit_code = 0;
    char inputSaved[256];
    char buffer[256];
    sprintf(inputSaved, "%s", input);
    const char delim[] = "&";
    char *token;
    token = strtok(input, delim);

    // Counting jobs in paralell.
    int counter = 0;
    while(token != NULL){
        token = strtok(NULL, delim);
        counter++;
    }

    // Putting jobs into array.
    char *args[counter];
    token = strtok(inputSaved, delim);
    for(int i = 0; i < counter; i++){
        // Trimming leading space.
        args[i] = token;
        token = strtok(NULL, delim);
    }
    
    // Running jobs.
    pid_t pids[counter];
    int out;
    for(int i = 0; i < counter; i++){
        if((pids[i] = fork()) < 0){
            perror("fork");
            abort();
        }else if(pids[i] == 0){
            exit(runCommand(args[i], dir, path));
        }
    }

    // Waiting for children to exit.
    int status;
    pid_t pid;
    while(counter > 0){
        pid = wait(&status);
        if(status == 256){
            exit_code = 1;
        }
        --counter;
    }

    return exit_code;
}