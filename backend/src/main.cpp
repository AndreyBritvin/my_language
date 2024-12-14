#include "my_log.h"
#include "my_tree.h"
#include "read_tree.h"
#include "make_assembler_file.h"
#include "name_table.h"

int main()
{
    enable_logging("log/backend.html");

    nametable_t nametable = {};
    create_name_table(&nametable);
    char *buffer = 0;
    fill_buffer(&buffer, "trees_binary/output_tree.txt");
    // printf("%s\n", buffer);

    my_tree_t proga_tree = make_prog_tree(buffer, nametable);
    TREE_DUMP(&proga_tree, proga_tree.root, "This is rebuild proga");
    print_name_table(nametable);

    generate_assembler(&proga_tree, "assembler_programs/test_asm.txt", nametable);

    delete_name_table(nametable);
    tree_dtor(&proga_tree);
    free(buffer);

    disable_logging();

    return EXIT_SUCCESS;
}
