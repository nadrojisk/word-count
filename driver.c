
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

char *read_file(char const *filename);
void send_file(char *buffer, int fd[2]);
void process_file(int fd[2]);

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("%s\n", "Usage: ./driver filename");
        return 1;
    }

    pid_t pid;
    int fd[2];

    if (pipe(fd) == -1)
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
        char *lp_buffer = read_file(argv[1]);
        // printf("%s\n", lp_buffer);
        send_file(lp_buffer, fd);
        free(lp_buffer);
    }

    else
    {
        // child
        process_file(fd);
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

void send_file(char *buffer, int fd[])
{
    close(fd[0]);

    write(fd[1], buffer, strlen(buffer));

    close(fd[1]);
}

void process_file(int fd[])
{
    char *lp_buffer = malloc(sizeof(lp_buffer) * 2048);
    close(fd[1]);

    read(fd[0], lp_buffer, 2048);
    printf("child read:\n%s\n", lp_buffer);

    close(fd[0]);
}