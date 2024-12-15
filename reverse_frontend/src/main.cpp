#include "my_log.h"
#include "my_tree.h"
#include "read_tree.h"
#include "make_prog_file.h"

int main(const int argc, const char** argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "Sorry, but programm should be %s <input> <output>\n", argv[0]);
        return EXIT_FAILURE;
    }
    enable_logging("log/backend.html");

    char *buffer = 0;
    fill_buffer(&buffer, argv[1]);
    // printf("%s\n", buffer);
    nametable_t nametable = {};
    create_name_table(&nametable);
    my_tree_t proga_tree = make_prog_tree(buffer, nametable);
    TREE_DUMP(&proga_tree, proga_tree.root, "This is rebuild proga");

    generate_prog_file(&proga_tree, argv[2]);

    delete_name_table(nametable);
    tree_dtor(&proga_tree);
    free(buffer);

    disable_logging();

    return EXIT_SUCCESS;
}
