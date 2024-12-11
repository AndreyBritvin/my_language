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

    char *buffer = NULL;
    if (fill_buffer(&buffer, argv[1]) == ERROR_CALLOC_IS_NULL)
    {
        return ERROR_CALLOC_IS_NULL;
    }
    printf("%s\n", buffer);

    my_tree_t programm_tree = make_tree(buffer);
    TREE_DUMP(&programm_tree, programm_tree.root, "This is out programm");

    overwrite_file(&programm_tree, "trees_binary/output_tree.txt");

    tree_dtor(&programm_tree);
    disable_logging();
    free(buffer);

    return 0;
}
