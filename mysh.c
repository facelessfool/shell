#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/wait.h>

//limits
#define MAX_TOKENS 100
#define MAX_STRING_LEN 100

size_t MAX_LINE_LEN = 10000;


// builtin commands
#define EXIT_STR "exit"
#define EXIT_CMD 0
#define UNKNOWN_CMD 99


FILE *fp; // file struct for stdin
char **tokens;
char *line;

void initialize()
{

	// allocate space for the whole line
	assert( (line = malloc(sizeof(char) * MAX_STRING_LEN)) != NULL);

	// allocate space for individual tokens
	assert( (tokens = malloc(sizeof(char*)*MAX_TOKENS)) != NULL);

	// open stdin as a file pointer 
	assert( (fp = fdopen(STDIN_FILENO, "r")) != NULL);

}


void fork_fn(){

	printf("this is a fork function \n");
	pid_t child_pid;
	child_pid=fork();

	printf("child_pid is storing %d",child_pid);

	if (child_pid == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
  }

  /* The parent is waiting for the child to complete its task. */
  if (child_pid == 0) {
    // int exec_return_value = execlp("/bin/ls", "ls", "-l", (char *) NULL);
    // if (exec_return_value == -1) {
    //   perror("execlp");
    //   exit(EXIT_FAILURE);
    // }

    execvp(tokens[0],tokens);
  }
  else {
    int wait_status;
    pid_t terminated_child_pid = wait(&wait_status);
    if (terminated_child_pid == -1) {
      perror("wait");
      exit(EXIT_FAILURE);
    }
    else {printf("Parent: my child %d terminates.\n", terminated_child_pid);}
    
    /* We can use the macro to examine the waiting status here. */
  }
  exit(EXIT_SUCCESS);

}

void tokenize (char * string)
{
	int token_count = 0;
	int size = MAX_TOKENS;
	char *this_token;

	while ( (this_token = strsep( &string, " \t\v\f\n\r")) != NULL) {

		if (*this_token == '\0') continue;

		tokens[token_count] = this_token;

		printf("Token %d: %s\n", token_count, tokens[token_count]);

		token_count++;

		// if there are more tokens than space ,reallocate more space
		if(token_count >= size){
			size*=2;

			assert ( (tokens = realloc(tokens, sizeof(char*) * size)) != NULL);
		}
	}

	fork_fn();
}

void read_command() 
{

	// getline will reallocate if input exceeds max length
	assert( getline(&line, &MAX_LINE_LEN, fp) > -1); 

	printf("Shell read this line: %s\n", line);

	tokenize(line);
}

int run_command() {

	if (strcmp( tokens[0], EXIT_STR ) == 0)
		return EXIT_CMD;

	return UNKNOWN_CMD;
}

int main()
{
	initialize();

	do {
		printf("sh550> ");
		read_command();
		
	} while( run_command() != EXIT_CMD );

	return 0;
}
