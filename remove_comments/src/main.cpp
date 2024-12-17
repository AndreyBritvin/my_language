#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

int main(const int argc, const char** argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "Sorry, but programm should be %s <input> <output>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE* SAFE_OPEN_FILE(input, argv[1], "r");
    FILE* SAFE_OPEN_FILE(output, argv[2], "w");

    SAFE_CALLOC(line, char, MAX_STRING_SIZE * 32);

    while (fgets(line, MAX_STRING_SIZE * 32, input) != NULL)
    {
        char* comm_begin = strchr(line, ';');
        if (line[MAX_STRING_SIZE * 32 - 2] != '\0')
        {
            fprintf(stderr, "You should increase MAX_STRING_SIZE * 32 or reduce your code\n");
            abort();
        }
        fwrite(line, sizeof(char), comm_begin == NULL ? strlen(line) : comm_begin - line, output);
        fprintf(output, "\n");
        line[0] = 0;
    }

    free(line);
    fclose(input);
    fclose(output);

    return EXIT_SUCCESS;
}
