#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

#define MAX_BUFFER 2048
//#define DEBUG

char *read_file(char const *filename);
void send_pipe(char *buffer, int *fd);
void process_file(int *fd1, int *fd2);
char *read_pipe(int fd[]);
int count(char *buffer);
int check_file(char const *filename);

int main(int argc, char const *argv[])
{	
	// print usuage for file
	if (argc != 2)
	{
		printf("%s\n", "Usage: ./driver filename");
		return 1;
	}
	
	// setup IPC information
	pid_t pid;
	int fd1[2];
	int fd2[2];

	if (pipe(fd1) == -1)
	{
		fprintf(stderr, "Pipe Failed");
		return 1;
	}
	if (pipe(fd2) == -1)
	{
		fprintf(stderr, "Pipe Failed");
		return 1;
	}
	
	// fork running process
	pid = fork();

	if (pid < 0)
	{
		fprintf(stderr, "Fork Failed");
		return 1;
	}

	if (pid > 0)
	{ 
		// parent will send the files contents to the child

		#ifdef DEBUG
			printf("%s\n", "Starting Parent...");
		#endif

		char *lp_buffer = read_file(argv[1]);
		send_pipe(lp_buffer, fd1);

		wait(NULL); // WAIT FOR CHILD
		
		// after child if finished executing parent will print out number of words
		#ifdef DEBUG
			printf("Resuming Parent...\n");
		#endif

		printf("Number of Words: %s\n", read_pipe(fd2));

		free(lp_buffer);
	}

	else
	{
		// child reads in file from parents and counts number of words
		#ifdef DEBUG
			printf("\n\n%s\n", "Starting Child...");
		#endif
		process_file(fd1, fd2);
		#ifdef DEBUG
			printf("Ending Child...\n\n\n");
		#endif
	}

	return 0;
}

char *read_file(char const *filename)
{
	// read provided file and return contents
	if (check_file(filename)){
		printf("Error: Provided file is not a text file. Quiting application\n");
		exit(1);
	}	
	FILE *pFile;
	pFile = fopen(filename, "r");

	// obtain file size:
	fseek(pFile, 0, SEEK_END);
	int lSize = ftell(pFile);
	rewind(pFile);

	char *lp_buffer = malloc(sizeof(lp_buffer) * lSize);

	fread(lp_buffer, 1, lSize, pFile);

	return lp_buffer;
}

int check_file(char const *filename){
	FILE *fp;

	char *command = malloc(sizeof(command) * MAX_BUFFER);
	char *path = malloc(sizeof(command) * MAX_BUFFER);

	sprintf(command, "file -b --mime-type %s", filename);
	fp = popen(command, "r");

	fgets(path, MAX_BUFFER, fp);
	#ifdef DEFINE
		printf("%s", path);
	#endif

	char *tok = strtok(path, "/");
	return strcmp("text", tok);	
}

void send_pipe(char *buffer, int *fd)
{	
	// send buffer through provided pipe
	
	close(fd[0]);
	#ifdef DEBUG
		printf("Sending: %s\n", buffer);
	#endif
	write(fd[1], buffer, strlen(buffer));

	close(fd[1]);
}

char *read_pipe(int fd[])
{
	// read information from provided pipe
	
	char *lp_buffer = malloc(sizeof(lp_buffer) * MAX_BUFFER);
	close(fd[1]);

	read(fd[0], lp_buffer, MAX_BUFFER);
	#ifdef DEBUG
		printf("Received: %s\n", lp_buffer);
	#endif
	close(fd[0]);

	return lp_buffer;
}

void process_file(int *fd1, int *fd2)
{
	// reads file from pipe, counts number of words, and sends
	// number back through pipe to parent
	
	char *lp_buffer = read_pipe(fd1);
	int counter = count(lp_buffer);
	free(lp_buffer);
	#ifdef DEBUG
		printf("Number of words: %d\n", counter);
	#endif

	char *str_int = malloc(sizeof(str_int) * sizeof(int));

	sprintf(str_int, "%d", counter);
	send_pipe(str_int, fd2);
	free(str_int);
}

int count(char *buffer)
{
	// count number of words in buffer
	
	int counter = 0;
	char *pch = strtok(buffer, " ");

	while (pch != NULL)
	{
		counter++;
		pch = strtok(NULL, " \n");
	}
	return counter;
}
