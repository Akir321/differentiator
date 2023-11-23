#include <stdio.h>

#include "tree_of_expressions.h"
#include "exp_tree_write.h"
#include "exp_tree_read.h"
#include "tree_graphic_dump.h"

const char *fileName = "exp_tree2.txt";

int main()
{
    Tree tree  = {};
    readTreeInfix(&tree, fileName);

    treeGraphicDump(&tree);

    printTreePrefix(&tree, tree.root, stdout);
    putchar('\n');
    printTreeInfix(&tree, tree.root, stdout);
    putchar('\n');
    printTreePostfix(&tree, tree.root, stdout);
    putchar('\n');

    ExpTreeErrors error = NO_ERROR;
    printf("expression = %lg\n", expTreeEvaluate(&tree, tree.root, &error));

    printTreeInfixNoUselessBrackets(&tree, tree.root, stdout);
    putchar('\n');

    nameTableDump(&tree.names, stdout);

    treeDtor(&tree);
}
