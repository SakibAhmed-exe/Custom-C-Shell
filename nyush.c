#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <libgen.h> 
#include <sys/wait.h>

void parser(char *commandLine, char **arguments) {
    char *token;
    char *subsequentCalls = NULL;
    size_t tokenSize;
    for (token = strtok_r(commandLine, " ", &subsequentCalls); token != NULL; token = strtok_r(NULL, " ", &subsequentCalls)) {
        tokenSize = strlen(token);
        if (token[tokenSize - 1] == '\n') {
            token[--tokenSize] = '\0';
        }
        *arguments++ = token;
    }
    *arguments = (void*)'\0';
}

void executed(char **arguments) {
    for(int m = 0; arguments[m] != NULL; m++) {
        int size = strlen(arguments[m]);
        for(int n = 0; n < size; n++) {
            if(arguments[m][n] == '/') {
                if((arguments[m][0] == '/') || ((arguments[m][0] == '.') && (arguments[m][1] == '/')))  {
                    break;
                }
                else {
                    char *newStr = (char*) malloc(strlen(arguments[m] + 3));
                    strcpy(newStr, "./");
                    strcat(newStr, arguments[m]);
                    arguments[m] = (char*) malloc(strlen(arguments[m] + 3));
                    strcpy(arguments[m], newStr);
                    break;
                }
            }
            else {
                continue;
            }
        }
    }
    void executeBasic();
    executeBasic(arguments);
    return;
}

void executeBasic(char **arguments) {
    pid_t pid;
    pid = fork();

    if(pid < 0) {
        fprintf(stderr, "Error: fork failed\n");
        fflush(stdout);
        return;
    }
    else if(pid == 0) {
        void sigRevert();
        sigRevert();
        int execSuccess = execvp(arguments[0], arguments);
        if(execSuccess == -1) {
            int secondExec = execvp(arguments[0], arguments);
            if(secondExec == -1) {
                fprintf(stderr, "Error: invalid program\n");
                fflush(stdout);
                exit(2);
                return;
            }
        }
    }
    else {
        int status;
        void sigIgnore();
        sigIgnore();
        waitpid(pid, &status, WUNTRACED);
    }
    return;
}

void sigRevert(){
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);
}

void sigIgnore(){
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
}

int main() {
    char absolutePath[256];
    char *arguments[100];
    char *commandLine = NULL;
    size_t commandLineSize = 0;

    while(1) {
        void sigIgnore();
        sigIgnore();

        if (getcwd(absolutePath, sizeof(absolutePath)) == NULL) {
            perror("getcwd() error");
        }
        else {
            printf("[nyush %s]$ ", basename(absolutePath));
            fflush(stdout);
        }

        fflush(stdout);
        getline(&commandLine, &commandLineSize, stdin);
        parser(commandLine, arguments);
        int i = 0;
        int j = 0;
        int redirectIns = 0;
        int redirectOuts = 0;
        int wrongRedirects = 0;
        int fileNamesIn = 0;
        int fileIndexIn = -2;
        int fileNamesOut = 0;
        int fileIndexOut = -2;
        int pipes = 0;
        for(i = 0; arguments[i] != NULL; i++) {
            j = i;
            if(strcmp(arguments[i], "|") == 0) {
                pipes++;
            }
            if(redirectIns > 0) {
                if(strcmp(arguments[i-1],"<") == 0) {
                    fileIndexIn = i;
                    fileNamesIn++;
                }  
            }
            if(fileIndexIn == (i-1)) {
                if(strcmp(arguments[i], "<") != 0 && strcmp(arguments[i], ">") != 0) {
                    if(strcmp(arguments[i], ">>") != 0 && strcmp(arguments[i], "|") != 0) {
                        if(strcmp(arguments[i], " ") != 0) {
                            fileNamesIn++;
                        }
                    }
                }
            }
            if(redirectOuts > 0) {
                if(strcmp(arguments[i-1],">") == 0) {
                    fileIndexOut = i;
                    fileNamesOut++;
                }  
            }
            if(fileIndexOut == (i-1)) {
                if(strcmp(arguments[i], "<") != 0 && strcmp(arguments[i], ">") != 0) {
                    if(strcmp(arguments[i], ">>") != 0 && strcmp(arguments[i], "|") != 0) {
                        if(strcmp(arguments[i], " ") != 0) {
                            fileNamesOut++;
                        }
                    }
                }
            }
            if(strcmp(arguments[i], "<") == 0) {
                redirectIns++;
            }
            if(strcmp(arguments[i], ">") == 0 || strcmp(arguments[i], ">>") == 0) {
                redirectOuts++;
            }
            if(strcmp(arguments[i], "<<") == 0) {
                wrongRedirects++;
            }
        }
        if(fileNamesIn > 1 || fileNamesOut > 1) {
            fprintf(stderr, "Error: invalid command\n");
            fflush(stdout);
        }
        else if(redirectIns > 1 || redirectOuts > 1 ) {
            fprintf(stderr, "Error: invalid command\n");
            fflush(stdout);
        }
        else if(wrongRedirects > 0) {
            fprintf(stderr, "Error: invalid command\n");
            fflush(stdout);
        }
        else if(strcmp(arguments[j], "<") == 0) {
            fprintf(stderr, "Error: invalid command\n");
            fflush(stdout);
        }
        else if(strcmp(arguments[j], ">") == 0) {
            fprintf(stderr, "Error: invalid command\n");
            fflush(stdout);
        }
        else if(strcmp(arguments[j], "|") == 0) {
            fprintf(stderr, "Error: invalid command\n");
            fflush(stdout);
        }
        else if(strcmp(arguments[0], "<") == 0) {
            fprintf(stderr, "Error: invalid command\n");
            fflush(stdout);
        }
        else if(strcmp(arguments[0], ">") == 0) {
            fprintf(stderr, "Error: invalid command\n");
            fflush(stdout);
        }
        else if(strcmp(arguments[0], "|") == 0) {
            fprintf(stderr, "Error: invalid command\n");
            fflush(stdout);
        }
        else if(pipes > 0) {
            fprintf(stderr, "Error: this command uses pipes which has not been implemented in this shell\n");
            fflush(stdout);
        }
        else if(strcmp(arguments[0], "fg") == 0 && j > 1) {
            fprintf(stderr, "Error: invalid command\n");
            fflush(stdout);
        }
        else if(strcmp(arguments[0], "jobs") == 0 && j > 0) {
            fprintf(stderr, "Error: invalid command\n");
            fflush(stdout);
        }
        else if (strcmp(arguments[0], "exit") == 0 && j != 0) { 
            fprintf(stderr, "Error: invalid command\n");
            fflush(stdout); 
        }
        else if (strcmp(arguments[0], "exit") == 0) { 
            exit(0);  
        }
        else if(strcmp(arguments[0], "cd") == 0 && j == 1) {
            if(chdir(arguments[1]) == -1) {
                fprintf(stderr, "Error: invalid directory\n");
                fflush(stdout);  
            }
        }
        else if (strcmp(arguments[0], "cd") == 0 && j != 1) {
            fprintf(stderr, "Error: invalid command\n");
            fflush(stdout);
        }
        else{
            executed(arguments);  
        } 
        j = 0; 
        redirectIns = 0;
        redirectOuts = 0;
        wrongRedirects = 0;
        fileIndexIn = 0;
        fileIndexOut = 0;
        fileNamesIn = 0;
        fileNamesOut = 0;
        pipes = 0;
    } 
} 
