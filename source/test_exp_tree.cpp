#include <stdio.h>

#include "tree_of_expressions.h"
#include "exp_tree_write.h"
#include "exp_tree_read.h"
#include "tree_graphic_dump.h"

const char *fileName = "exp_tree.txt";

int main()
{
    Tree tree  = {};
    Node *root = readTreePrefix(fileName);
    tree.root = root;

    //treeGraphicDump(&tree);

    printTreePrefix(root, stdout);
    putchar('\n');
    printTreeInfix(root, stdout);
    putchar('\n');
    printTreePostfix(root, stdout);
    putchar('\n');

    ExpTreeErrors error = NO_ERROR;
    printf("expression = %lg\n", expTreeEvaluate(tree.root, &error));

    printTreeInfixNoUselessBrackets(tree.root, stdout);
    putchar('\n');

    treeDtor(tree.root);
}
