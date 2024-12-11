#include "my_log.h"
#include "my_tree.h"
#include "read_tree.h"

int main()
{
    enable_logging("log/backend.html");

    char *buffer = 0;
    fill_buffer(&buffer, "trees_binary/output_tree.txt");
    // printf("%s\n", buffer);

    my_tree_t proga_tree = make_prog_tree(buffer);
    TREE_DUMP(&proga_tree, proga_tree.root, "This is rebuild proga");

    tree_dtor(&proga_tree);
    free(buffer);

    return 0;
}
