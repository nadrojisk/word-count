
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

// #define DEBUG

char *read_file(char const *filename);
void send_pipe(char *buffer, int *fd);
void process_file(int *fd1, int *fd2);
char *read_pipe(int fd[]);

int count(char *buffer);

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("%s\n", "Usage: ./driver filename");
        return 1;
    }

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

    pid = fork();

    if (pid < 0)
    {
        fprintf(stderr, "Fork Failed");
        return 1;
    }

    if (pid > 0)
    { // parent

#ifdef DEBUG
        printf("%s\n", "Starting Parent...");
#endif

        char *lp_buffer = read_file(argv[1]);
        send_pipe(lp_buffer, fd1);
        wait(NULL); // WAIT FOR CHILD

#ifdef DEBUG
        printf("Resuming Parent...\n");
#endif

        printf("Number of Words: %s\n", read_pipe(fd2));

        free(lp_buffer);
    }

    else
    {
        // child
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

void send_pipe(char *buffer, int *fd)
{
    close(fd[0]);
#ifdef DEBUG
    printf("sending: %s\n", buffer);
#endif
    write(fd[1], buffer, strlen(buffer));

    close(fd[1]);
}

char *read_pipe(int fd[])
{

    char *lp_buffer = malloc(sizeof(lp_buffer) * 2048);
    close(fd[1]);

    read(fd[0], lp_buffer, 2048);
#ifdef DEBUG
    printf("received: %s\n", lp_buffer);
#endif
    close(fd[0]);

    return lp_buffer;
}

void process_file(int *fd1, int *fd2)
{
    char *lp_buffer = read_pipe(fd1);
    int counter = count(lp_buffer);

    char *str_int;
    sprintf(str_int, "%d", counter);
    send_pipe(str_int, fd2);
}

int count(char *buffer)
{
    int counter = 0;
    char *pch = strtok(buffer, " ");

    while (pch != NULL)
    {
        counter++;
        pch = strtok(NULL, " \n");
    }
    return counter;
}