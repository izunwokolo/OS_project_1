#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_COMMAND_LINE_LEN 1024
#define MAX_COMMAND_LINE_ARGS 128
#define PATH_MAX 4096
#define MAX_ENV_VARIABLE_NAME_LENGTH 64
#define MAX_ENV_VARIABLE_VALUE_LENGTH 1024


char prompt[] = "> ";
char delimiters[] = " \t\r\n";
extern char **environ;

void cancelHandler(int signum) {
	printf("\n");
}

void handler(int signum)//signal handler
{ 
	exit(0);
}

int main() {
    // Stores the string typed into the command line.
    char command_line[MAX_COMMAND_LINE_LEN];
    char cmd_bak[MAX_COMMAND_LINE_LEN];
		char cwd[PATH_MAX];
  
    // Stores the tokenized command line input.
    char *arguments[MAX_COMMAND_LINE_ARGS];
    	
    while (true) {
			memset(arguments, 0, MAX_COMMAND_LINE_ARGS);
      
        do{ 
            // Print the shell prompt.
						getcwd(cwd, sizeof(cwd));
            printf("%s%s", cwd, prompt);
            fflush(stdout);

            // Read input from stdin and store it in command_line. If there's an
            // error, exit immediately. (If you want to learn more about this line,
            // you can Google "man fgets")
        
            if ((fgets(command_line, MAX_COMMAND_LINE_LEN, stdin) == NULL) && ferror(stdin)) {
                fprintf(stderr, "fgets error");
                exit(0);
            }
 
        }while(command_line[0] == 0x0A);  // while just ENTER pressed

      
        // If the user input was EOF (ctrl+d), exit the shell.
        if (feof(stdin)) {
            printf("\n");
            fflush(stdout);
            fflush(stderr);
            return 0;
        }        
			  // 0. Modify the prompt to print the current working directory

        // 1. Tokenize the command line input (split it on whitespace)
				command_line[strlen(command_line) -1] = '\0'; // this removes the last new character at the end of the input string
				int i = 0;
				bool check = false;
				int argumentCount = 0;
				char* inputWord = strtok(command_line, delimiters);
				char copiedStr[MAX_COMMAND_LINE_LEN];
				char gottenVal[MAX_COMMAND_LINE_LEN];
				char* nextIndex;
				while (true){
						if(inputWord != NULL){
							if (inputWord[0] == '$' && strlen(inputWord) > 1 && argumentCount > 0){
								strcpy(copiedStr, inputWord + 1);
								check = true;
								char* envVal = getenv(copiedStr);
								if (envVal == NULL){
										strcpy(gottenVal, "");
								}
								else{
									strcpy(gottenVal, envVal);
								}
							}
						nextIndex = (char*) malloc(MAX_COMMAND_LINE_LEN * sizeof(char));
						strcpy(nextIndex, check ? gottenVal : inputWord);
						arguments[i++] = nextIndex;
						inputWord = strtok(NULL, delimiters);
						++argumentCount; 
				}
				else{
					break;
				}
			}    
        // 2. Implement Built-In Commands
				if (strcmp(arguments[0], "echo") == 0){
					int index;
					for (index = 1; index < argumentCount; ++index){
						if(index > 1){
							printf(" ");
						}
						printf("%s", arguments[index]);
					}
					printf("\n");
				}
				else if (strcmp(arguments[0], "pwd") == 0){
					printf("%s\n", cwd);
				}
				else if (strcmp(arguments[0], "cd") == 0){
					if (arguments[1] == NULL){
						chdir(getenv("HOME"));
					}
					else{
						chdir(arguments[1]);
						}
					}
				else if (strcmp(arguments[0], "exit") == 0){
					exit(0);
				}
				else if (strcmp(arguments[0], "setenv") == 0){
					char name[MAX_ENV_VARIABLE_NAME_LENGTH];
					char value[MAX_ENV_VARIABLE_VALUE_LENGTH];
					char* p = strtok(arguments[1], "=");
					strcpy(name, p);
					while (p != NULL) {
						strcpy(value, p);
						p = strtok(NULL, "=");
					}
					setenv(name, value, 1);
				}	
				else if (strcmp(arguments[0], "env") == 0) {
					if (argumentCount == 1) {
						char ** envs = environ;
						for (;* envs; envs++) {
							printf("%s\n", * envs);
						}
					} else {
						char * env_value = getenv(arguments[1]);
						if (env_value != NULL) {
							printf("%s\n", env_value);
						}
				}
      }
        // 3. Create a child process which will execute the command line input
				else{
					pid_t  pid;
     			pid = fork();
					char* cmd = arguments[0];
					int isinBackground = 0;
					if (strcmp(arguments[argumentCount - 1], "&") == 0){
						arguments[argumentCount - 1] = NULL;
						isinBackground = 1;
					}
					if (pid == 0){
						signal(SIGALRM,handler);
						alarm(10);
						if (isinBackground){
						int fd = open("/dev/null", O_WRONLY);
						dup2(fd, STDOUT_FILENO);
						dup2(fd, STDERR_FILENO);
					}
					if (execvp(cmd, arguments) == -1){
					  printf("execvp() failed: No such file or directory. An error occurred\n");
					}
					alarm(0);
					exit(0);
				}
					else {
						if (isinBackground == 0){
							signal(SIGINT,cancelHandler);
							waitpid(pid, NULL, WUNTRACED);
						}
					}
    }
        // 4. The parent process should wait for the child to complete unless its a background process
        // Hints (put these into Google):
        // man fork
        // man execvp
        // man wait
        // man strtok
        // man environ
        // man signals
        
        // Extra Credit
        // man dup2
        // man open
        // man pipes
    }
    // This should never be reached.
    return -1;
}
