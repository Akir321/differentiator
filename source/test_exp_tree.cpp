#include <stdio.h>

#include "tree_of_expressions.h"
#include "exp_tree_write.h"
#include "exp_tree_read.h"
#include "tree_graphic_dump.h"

const char *fileName = "exp_tree2.txt";

int main()
{
    Evaluator evaluator = {};
    evaluatorCtor(&evaluator);
    readTreeInfix(&evaluator, fileName);

    treeGraphicDump(&evaluator);

    printTreePrefix(&evaluator, evaluator.tree.root, stdout);
    putchar('\n');
    printTreeInfix(&evaluator, evaluator.tree.root, stdout);
    putchar('\n');
    printTreePostfix(&evaluator, evaluator.tree.root, stdout);
    putchar('\n');

    ExpTreeErrors error = NO_ERROR;
    printf("expression = %lg\n", expTreeEvaluate(&evaluator, evaluator.tree.root, &error));

    printTreeInfixNoUselessBrackets(&evaluator, evaluator.tree.root, stdout);
    putchar('\n');

    nameTableDump(&evaluator.names, stdout);

    evaluatorDtor(&evaluator);
}
