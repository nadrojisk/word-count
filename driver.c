#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("%s\n", "Usage: ./driver filename");
        exit(1);
    }

    FILE *pFile;
    pFile = fopen(argv[1], "r");

    // obtain file size:
    fseek(pFile, 0, SEEK_END);
    int lSize = ftell(pFile);
    rewind(pFile);

    char buffer[lSize];
    char *lp_buffer = buffer;

    fread(lp_buffer, 1, lSize, pFile);

    printf("%s\n", lp_buffer);
    return 0;
}
