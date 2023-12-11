#include <stdio.h>
#include <assert.h>

#include "tree_of_expressions.h"
#include "differentiator.h"
#include "exp_tree_write.h"
#include "prog_differentiator.h"
#include "html_logfile.h"
#include "tree_graphic_dump.h"

extern const char *Introduction;
extern const char *Simplification;
extern const char *Deriv2;
extern const char *Simplification2;
extern const char *Taylor;
extern const char *Deriv3;
extern const char *Taylor2;
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
    treeGraphicDump(&deriv1, deriv1.tree.root);

    Evaluator deriv2 = {};
    secondDerivative(&deriv1, &deriv2);

    Evaluator deriv3 = {};
    thirdDerivative(&deriv2, &deriv3);
    
    taylor(function, &deriv1, &deriv2, &deriv3);

    PRINT_TO_TEX(Conclusion);
    PRINT_TO_TEX(References);

    evaluatorDtor(&deriv1);
    evaluatorDtor(&deriv2);

    return EXIT_SUCCESS;
}

int firstDerivative(Evaluator *function, Evaluator *deriv1)
{
    differentiate(function, deriv1, true);
    treeGraphicDump(deriv1, deriv1->tree.root);

    expTreeSimplify(deriv1, deriv1->tree.root);
    PRINT_TO_TEX(Simplification);
    printTreeToTexFile(deriv1, deriv1->tree.root, TexFile, NULL);

    return EXIT_SUCCESS;
}

int secondDerivative(Evaluator *deriv1, Evaluator *deriv2)
{
    PRINT_TO_TEX(Deriv2);
    differentiate(deriv1, deriv2, true);

    PRINT_TO_TEX(Simplification2);
    expTreeSimplify(deriv2, deriv2->tree.root);
    printTreeToTexFile(deriv2, deriv2->tree.root, TexFile, NULL);

    return EXIT_SUCCESS;
}

int thirdDerivative(Evaluator *deriv2, Evaluator *deriv3)
{
    PRINT_TO_TEX(Taylor);
    differentiate(deriv2, deriv3, false);

    PRINT_TO_TEX(Deriv3);
    expTreeSimplify(deriv3, deriv3->tree.root);

    return EXIT_SUCCESS;
}

#define SET_X(value) \
    nameTableSetValue(&function->names, "x", value); \
    nameTableSetValue(&deriv1->names,   "x", value); \
    nameTableSetValue(&deriv2->names,   "x", value); \
    nameTableSetValue(&deriv3->names,   "x", value); 

int taylor(Evaluator *function, Evaluator *deriv1, Evaluator *deriv2, Evaluator *deriv3)
{
    PRINT_TO_TEX(Taylor2);
    ExpTreeErrors error = TREE_NO_ERROR;
    double func = 0, d1 = 0, d2 = 0, d3 = 0;
    int x0 = -1;

    do
    {
        error = TREE_NO_ERROR;
        x0++;
        SET_X(x0);

        func = expTreeEvaluate(function, function->tree.root, &error);
        d1   = expTreeEvaluate(deriv1,   deriv1->tree.root,   &error);
        d2   = expTreeEvaluate(deriv2,   deriv2->tree.root,   &error);
        d3   = expTreeEvaluate(deriv3,   deriv3->tree.root,   &error);

    }   while (error && x0 < 10);

    LOG("error (of evaluating) = %d\n", error);
    if (error)  PRINT_TO_TEX(NotDefined);
    else 
    {
        fprintf(TexFile, "\n\n$x_0 = %d$\n\n$f(x_0) = %lg$\n\n$f'(x_0) = %lg$\n\n$f''(x_0) = %lg$\n\n$f'''(x_0) = %lg$\n\n", 
                    x0, func, d1, d2, d3);
        //fprintf(TexFile, " $f(x) = (%lg) + (%lg)x + (%lg)x^2 + (%lg)x^3 +  o(x^3)$ \n\n", func, d1, d2/2, d3/6);
        taylorMakePics(function, func, d1, d2, d3);
    }

    return EXIT_SUCCESS;
}


#define GNUPLOT_FORMAT "set xlabel \"X\"\nset ylabel \"Y\"\n\nset grid\nset xrange[%lg:%lg]\n\nset title \"Approximation\" font \"Helvetica Bold, 20\"\nset terminal png size 800, 600\nset output \"gnuplot/graph%d.png\"\n\n"

int taylorMakePics(Evaluator *function, double func, double d1, double d2, double d3)
{
    fprintf(TexFile, "Итак, приступим:\n\n");

    printTaylor(1, function, func, d1, 0, 0);
    gnuplotMakeGraph(1, function, func, d1, 0, 0);

    printTaylor(2, function, func, d1, d2 / 2, 0);
    gnuplotMakeGraph(2, function, func, d1, d2 / 2, 0);

    printTaylor(3, function, func, d1, d2 / 2, d3 / 6);
    gnuplotMakeGraph(3, function, func, d1, d2 / 2, d3 / 6);

    return EXIT_SUCCESS;
}

#define PRINT_KOEFF(koeff, kNum, o, prev)                       \
    if (!equalDouble(koeff, 0))                                 \
    {                                                           \
        if (koeff < 0)    fprintf(TexFile, "%lg" o " ", koeff); \
                                                                \
        if (!equalDouble(prev, 0)) fprintf(TexFile, " + ");     \
                                                                \
        if (equalDouble(koeff, 1)) fprintf(TexFile,  o " ");    \
        else if (koeff > 0)        fprintf(TexFile, " %lg" o " ", koeff);\
    }                                                           \
    if (power == kNum)                                          \
    {                                                           \
        fprintf(TexFile, "+o(" o ")$\n\n");                     \
        return EXIT_SUCCESS;                                    \
    }                                           

int printTaylor(int power, Evaluator *function, double func, double k1, double k2, double k3)
{
    assert(function);

    fprintf(TexFile, "$f(x) = ");
    if (!equalDouble(func, 0)) fprintf(TexFile, "%lg ", func);

    PRINT_KOEFF(k1, 1, "x", 0);
    PRINT_KOEFF(k2, 2, "x^2", k1);
    PRINT_KOEFF(k3, 3, "x^3", k2);

    return EXIT_SUCCESS;
}

#undef PRINT_KOEFF

int gnuplotMakeGraph(int power, Evaluator *function, double func, double k1, double k2, double k3)
{
    int x0 = (int) function->names.table[nameTableFind(&function->names, "x")].value;
    const char *color = NULL;

    switch (power)
    {
        case 1: color = "orange"; break;
        case 2: color = "green";  break;
        case 3: color = "blue";   break;
    
        default: LOG("ERROR: unknown taylor power: %d\n", power); break;
    }

    char *fileName = NULL;
    __mingw_asprintf(&fileName, "gnuplot/graph%d.gpi", power);
    FILE *f = fopen(fileName, "w");

    fprintf(f, GNUPLOT_FORMAT, x0 - 1.0, x0 + 1.0, power);
    fprintf(f, "plot ");
    printTreeInfixNoUselessBrackets(function, function->tree.root, f);
    fprintf(f, " title \"function\" lc rgb \"red\", ");
    
    fprintf(f, "%lg + (%lg)*x + (%lg)*x**2 + (%lg)*x**3 title \"taylor\" lc rgb \"%s\"", func, k1, k2, k3, color);
    fclose(f);
    free(fileName);

    char *command = NULL;
    __mingw_asprintf(&command, "gnuplot gnuplot/graph%d.gpi", power);
    system(command);
    printf("%s\n", command);
    free(command);

    fprintf(TexFile, "\\includegraphics[width=1\\linewidth]{gnuplot/graph%d.png}\n\n", power);

    return EXIT_SUCCESS;
}
