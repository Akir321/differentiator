#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "tree_of_expressions.h"
#include "differentiator.h"
#include "exp_tree_read.h"
#include "prog_differentiator.h"
#include "tree_graphic_dump.h"


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

    Evaluator function = {};
    evaluatorCtor(&function);

    if (readTreeInfix(&function, fileInName)) { perror("couldn't read tree:"); return 0; }
    treeGraphicDump(&function, function.tree.root);

    TexFile = fopen(fileOutName,  "w");
    if (!TexFile) { perror("couldn't open output file:"); return 0; }

    runDifferentiator(&function);

    evaluatorDtor(&function);
    fclose(TexFile);
    free(fileOutName);
}

//.\differentiator.exe trees_to_read/exp_tree_pow.txt
//.\differentiator.exe trees_to_read/exp_tree_logar.txt
//.\differentiator.exe trees_to_read/exp_tree_ded.txt
