#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "tree_of_expressions.h"
#include "exp_tree_read.h"
#include "exp_tree_write.h"

#include "differentiator.h"

extern const char *Introduction;
extern const char *Simplification;
extern const char *Taylor;
extern const char *References;

FILE *TexFile = {};

int main(int argc, const char *argv[])
{
    srand((unsigned) time(NULL));

    const char *fileInName  = NULL;
    char       *fileOutName = NULL;

    if (processArgv(argc, argv, &fileInName, &fileOutName))
    {
        printf("ERROR: no file name given\n");
        return 0;
    }

    Evaluator eval = {};
    evaluatorCtor(&eval);

    if (readTreeInfix(&eval, fileInName)) { perror("couldn't read tree:"); return 0; }

    TexFile = fopen(fileOutName,  "w");
    if (!TexFile) { perror("couldn't open output file:"); return 0; }

    fprintf(TexFile, "%s\n", Introduction);

    fprintf(TexFile, "Давайте разберем такой несложный пример. Рассмотрим функцию ");
    printTreeToTexFile(&eval, eval.tree.root, TexFile, NULL);
    fprintf(TexFile,  " и найдем ее производную.\n\n");

    Evaluator deriv1 = {};
    differentiate(&eval, &deriv1);

    expTreeSimplify(&deriv1, deriv1.tree.root);
    printTreeToTexFile(&deriv1, deriv1.tree.root, TexFile, Simplification);

    fprintf(TexFile, "\n%s\n\n", Taylor);
    fprintf(TexFile, "%s\n", References);

    evaluatorDtor(&eval);
    evaluatorDtor(&deriv1);
    fclose(TexFile);
    free(fileOutName);
}