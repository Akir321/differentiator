#include <stdio.h>

#include "tree_of_expressions.h"
#include "exp_tree_write.h"
#include "exp_tree_read.h"
#include "tree_graphic_dump.h"
#include "recursive_descent_reading.h"

#include "differentiator.h"

const char *fileName = "trees_to_read/exp_tree_ded.txt";

const char *fileTex  = "exp_tree.tex";

//const char *Expression = "-(  - 314 +10  )";
//const char *Expression = " 100-  23*3-14  /7";
const char *Expression = "1000   -7 *100 /-(      30\n\n+5 *10+-5*(100/50)) +1 ";

//FILE *TexFile = fopen("dump_deriv.tex", "w");

int main()
{
    //Token *tokenArray = createTokenArray(Expression);
    //printTokenArray(tokenArray, stdout);

    Evaluator eval = {};
    evaluatorCtor(&eval);

    eval.tree.root = getG(Expression);
    treeGraphicDump(&eval, eval.tree.root);

    printTreeInfixNoUselessBrackets(&eval, eval.tree.root, stdout);
    putchar('\n');

    ExpTreeErrors error = TREE_NO_ERROR;
    printf("val = %lg\n", expTreeEvaluate(&eval, eval.tree.root, &error));

    evaluatorDtor(&eval);
}


/*
int main()
{

    Evaluator evaluator = {};
    evaluatorCtor(&evaluator);
    //readTreeInfix(&evaluator, fileName);

    evaluator.tree.root = getG("1000-7*100/(30+5*10-5*(100/50))+1");

    treeGraphicDump(&evaluator, evaluator.tree.root);

    printTreePrefix(&evaluator, evaluator.tree.root, stdout);
    putchar('\n');

    printTreeInfixNoUselessBrackets(&evaluator, evaluator.tree.root, stdout);
    putchar('\n');

    nameTableDump(&evaluator.names, stdout);

    Evaluator deriv = {};
    evaluatorCtor(&deriv);

    treeCtor(&deriv.tree, derivative(&evaluator, evaluator.tree.root));
    nameTableCopy(&evaluator.names, &deriv.names);

    treeGraphicDump(&deriv, deriv.tree.root);
    printTreeInfixNoUselessBrackets(&deriv, deriv.tree.root, stdout);
    putchar('\n');

    ExpTreeErrors error = TREE_NO_ERROR;
    printf("expression = %lg\n", expTreeEvaluate(&evaluator, evaluator.tree.root, &error));

    nameTableDump(&deriv.names, stdout);

    //
    expTreeSimplifyConsts(&deriv, deriv.tree.root);
    treeGraphicDump(&deriv);
    printTreeInfixNoUselessBrackets(&deriv, deriv.tree.root, stdout);
    putchar('\n');
    

    expTreeSimplify(&deriv, deriv.tree.root);
    treeGraphicDump(&deriv, deriv.tree.root);
    printTreeInfixNoUselessBrackets(&deriv, deriv.tree.root, stdout);
    putchar('\n');

    //
    FILE *f = fopen(fileTex, "w");
    printTreeTex(&evaluator, evaluator.tree.root, f);
    putc('\n', f);
    printTreeTex(&deriv, deriv.tree.root, f);
    

    evaluatorDtor(&evaluator);
    evaluatorDtor(&deriv);
    //fclose(f);
}
*/

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
