#include <stdio.h>
#include <assert.h>

#include "tree_of_expressions.h"
#include "differentiator.h"
#include "exp_tree_write.h"
#include "prog_differentiator.h"
#include "html_logfile.h"

extern const char *Introduction;
extern const char *Simplification;
extern const char *Deriv2;
extern const char *Simplification2;
extern const char *Taylor;
extern const char *NotDefined;
extern const char *Conclusion;
extern const char *References;

extern FILE *TexFile;

#define PRINT_TO_TEX(str) fprintf(TexFile, "%s", str)

int runDifferentiator(Evaluator *function)
{
    PRINT_TO_TEX(Introduction);

    PRINT_TO_TEX("Давайте разберем такой несложный пример. Рассмотрим функцию \n\n$f(x) = $ ");
    printTreeToTexFile(function, function->tree.root, TexFile, NULL);
    PRINT_TO_TEX(" и найдем ее производную.\n\n");
    PRINT_TO_TEX("Мы начнём с вычисления элементарных производных и постепенно дойдем до всей функции.\n\n");

    Evaluator deriv1 = {};
    firstDerivative(function, &deriv1);

    Evaluator deriv2 = {};
    secondDerivative(&deriv1, &deriv2);
    
    taylor(function, &deriv1, &deriv2);

    PRINT_TO_TEX(Conclusion);
    PRINT_TO_TEX(References);

    evaluatorDtor(&deriv1);
    evaluatorDtor(&deriv2);

    return EXIT_SUCCESS;
}

int firstDerivative(Evaluator *function, Evaluator *deriv1)
{
    differentiate(function, deriv1);

    expTreeSimplify(deriv1, deriv1->tree.root);
    PRINT_TO_TEX(Simplification);
    printTreeToTexFile(deriv1, deriv1->tree.root, TexFile, NULL);

    return EXIT_SUCCESS;
}

int secondDerivative(Evaluator *deriv1, Evaluator *deriv2)
{
    PRINT_TO_TEX(Deriv2);
    differentiate(deriv1, deriv2);

    PRINT_TO_TEX(Simplification2);
    expTreeSimplify(deriv2, deriv2->tree.root);
    printTreeToTexFile(deriv2, deriv2->tree.root, TexFile, NULL);

    return EXIT_SUCCESS;
}

int taylor(Evaluator *function, Evaluator *deriv1, Evaluator *deriv2)
{
    PRINT_TO_TEX(Taylor);

    ExpTreeErrors error = TREE_NO_ERROR;

    double func = expTreeEvaluate(function, function->tree.root, &error);
    double d1   = expTreeEvaluate(deriv1,   deriv1->tree.root,   &error);
    double d2   = expTreeEvaluate(deriv2,   deriv2->tree.root,   &error);

    LOG("error (of evaluating) = %d\n", error);

    if (error)  PRINT_TO_TEX(NotDefined);
    else        fprintf(TexFile, " $f(x) = (%lg) + (%lg)x + (%lg)x^2 + o(x^2)$ \n\n", func, d1, d2);

    return EXIT_SUCCESS;
}
