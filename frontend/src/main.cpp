#include <stdio.h>
#include <my_tree.h>
#include "my_log.h"
#include "recursive_descent_parser.h"

int main(int argc, char** argv)
{
    enable_logging("log/log.html");

    if (argc != 3)
    {
        fprintf(stderr, "Sorry, but programm should be %s <input> <output>\n", argv[0]);
    }

    char *buffer = fill_buffer(&buffer, "expressions/expr_1.txt");
    printf("%s\n", buffer);

    disable_logging();
    free(buffer);

    return 0;
}
