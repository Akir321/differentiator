#ifndef  __PROG_DIFFERENTIATOR_H__
#define  __PROG_DIFFERENTIATOR_H__

#include "tree_of_expressions.h"

int runDifferentiator(Evaluator *function);

int firstDerivative (Evaluator *function, Evaluator *deriv1);
int secondDerivative(Evaluator *deriv1,   Evaluator *deriv2);
int thirdDerivative (Evaluator *deriv2,   Evaluator *deriv3);

int taylorMakePics(Evaluator *function, double func, double d1, double d2, double d3);

int gnuplotMakeGraph(int power, Evaluator *function, double func, double k1, double k2, double k3);

int printTaylor(int power, Evaluator *function, double func, double k1, double k2, double k3);

int taylor(Evaluator *function, Evaluator *deriv1, Evaluator *deriv2, Evaluator *deriv3);

#endif //__PROG_DIFFERENTIATOR_H__
