#include <stdio.h>

#include "tree_of_expressions.h"
#include "exp_tree_write.h"
#include "exp_tree_read.h"
#include "tree_graphic_dump.h"

#include "differentiator.h"

const char *fileName = "trees_to_read/exp_tree_sin.txt";

const char *fileTex  = "exp_tree.tex";

FILE *TexFile = fopen("dump_deriv.tex", "w");

int main()
{

    Evaluator evaluator = {};
    evaluatorCtor(&evaluator);
    readTreeInfix(&evaluator, fileName);

    treeGraphicDump(&evaluator);

    printTreePrefix(&evaluator, evaluator.tree.root, stdout);
    putchar('\n');

    printTreeInfixNoUselessBrackets(&evaluator, evaluator.tree.root, stdout);
    putchar('\n');

    nameTableDump(&evaluator.names, stdout);

    Evaluator deriv = {};
    evaluatorCtor(&deriv);

    treeCtor(&deriv.tree, derivative(&evaluator, evaluator.tree.root));
    nameTableCopy(&evaluator.names, &deriv.names);

    treeGraphicDump(&deriv);
    printTreeInfixNoUselessBrackets(&deriv, deriv.tree.root, stdout);
    putchar('\n');

    nameTableDump(&deriv.names, stdout);

    /*
    expTreeSimplifyConsts(&deriv, deriv.tree.root);
    treeGraphicDump(&deriv);
    printTreeInfixNoUselessBrackets(&deriv, deriv.tree.root, stdout);
    putchar('\n');
    */

    expTreeSimplify(&deriv, deriv.tree.root);
    treeGraphicDump(&deriv);
    printTreeInfixNoUselessBrackets(&deriv, deriv.tree.root, stdout);
    putchar('\n');

    FILE *f = fopen(fileTex, "w");
    printTreeTex(&evaluator, evaluator.tree.root, f);
    putc('\n', f);
    printTreeTex(&deriv, deriv.tree.root, f);

    evaluatorDtor(&evaluator);
    evaluatorDtor(&deriv);
    fclose(f);
}

/*
printTreePrefix(&evaluator, evaluator.tree.root, stdout);
    putchar('\n');
    printTreeInfix(&evaluator, evaluator.tree.root, stdout);
    putchar('\n');
    printTreePostfix(&evaluator, evaluator.tree.root, stdout);
    putchar('\n');

    ExpTreeErrors error = NO_ERROR;
    printf("expression = %lg\n", expTreeEvaluate(&evaluator, evaluator.tree.root, &error));
*/
