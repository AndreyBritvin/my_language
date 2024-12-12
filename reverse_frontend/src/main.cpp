#include "my_log.h"
#include "my_tree.h"
#include "read_tree.h"
#include "make_prog_file.h"

int main()
{
    enable_logging("log/backend.html");

    char *buffer = 0;
    fill_buffer(&buffer, "trees_binary/output_tree.txt");
    // printf("%s\n", buffer);

    my_tree_t proga_tree = make_prog_tree(buffer);
    TREE_DUMP(&proga_tree, proga_tree.root, "This is rebuild proga");

    generate_prog_file(&proga_tree, "rebuild_codes/test_output.txt");

    tree_dtor(&proga_tree);
    free(buffer);

    disable_logging();

    return EXIT_SUCCESS;
}
