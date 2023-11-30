#ifndef  __LATEX_PHRASES_H__
#define  __LATEX_PHRASES_H__

#include <stdlib.h>
#include <stdio.h>
#include"tree_of_expressions.h"

#define SIZEAR(array) sizeof(array) / sizeof(array[0])

const char *getPrefix(Node *node);

const char *Introduction = 
    "\\documentclass{article}\n\\usepackage[russian]{babel}\n\n\\title{Вычисление производной}\n\\author{Авторы статьи возжелали остаться в тени}\n\\date{Время - иллюзия}\n\n\\begin{document}\n\n\\maketitle\n\n\\section{Вступление}\n\nУ современного человека в жизни часто возникает потребность дифференцировать. Бывает, что человек не может даже сопротивляться этому порыву: дай продифференцировать что-то или умру! Главная проблема заключается в том, что не каждый человек от рождения умеет вычислять производные функций. Авторы статьи решили помочь обездоленным и предоставить пример дифференцирования одной из функций, с помощью которого любой, даже самый отпет гуманитарий, смог бы научиться брать простенькие производные.\n\nНесмотря на свои добрые побуждения, авторы статьи не могут знать, к чему приведет ее использование. Авторы статьи не несут ответсвенности за последствия. Всё на вашей совести.\n\n\\section{Вычисление производной}\n\n";


const char *Simplification = 
   "Очевидно, что на данное выражение без слез не взглянешь. Авторы считают, что его необходимо упростить. Путем тривиальных преобразований получим:\n";


const char *Taylor = 
    "\\section{Размышления об исследовании функции}\n\nДля закрепления знаний, полученных в данной статье, авторы статьи предлагают читателю вычислить первую, вторую, третью, четвертую и пятую производные в точке $x_0$ в качестве упражнения. Зная эти производные, можно получить разложение функции по формуле Тейлора в окрестности данной точки. Авторы статьи надеятся, что данное упражнение не вызовет у читателя затруднений.\n\nФормула Тейлор имеет следующий вид:\n$$f(x) = f(x_0) + \\frac{f'(x_0)}{1!}(x - x_0) + \\cdots + \\frac{f^{(n)}(x_0)}{n!}(x - x_0)^n + o((x - x_0)^n).$$\n\nЧтобы упростить читателю задачу, авторы статьи вычислили первую и вторую производную в точке 0.\n\n";

const char *References = 
    "\\section*{Источники}\n\\begin{enumerate}\n\t\\item Любовь к математике;\n\t\\item Горящие сердца авторов статьи;\n\t\\item Божественные знания, данные нам Матом и Аном.\n\\end{enumerate}\n\n\\end{document}\n";


const char *General[] = {   "Очевидно, что",
                            "Любой школьник знает, что",
                            "Ещё в советских яслях проходили, что",
                            "Авторы статьи вопрошают: как посчитать эту производную? Ответ убил:",
                            "Бери от жизни все, даже производную:",
                            "Хотите брать -- надо брать!",
                            "Вы что, не хотите брать производную? Что бы вы делали без авторов статьи:",
                            "Вычислим производную данного выражения:",
                            "Если бы вас поставили перед выбором, взять производную или умереть, что бы вы выбрали?",
                            "Конец близок, он где-то в этой производной:",
                            "Производящее хозяйство нынче не в моде. Произведем хотя бы функцию",
                            "Один из авторов статьи решил, что вычислить эту производную довольно легко. Другие не согласны -- это же тривиально!",
                            "Что ещё уготовит на судьба?",
                            "О, великие Мат и Ан, спасите наши души!",
                            "Авторы статьи сомневаются в её необходимости. Это же и так очевидно:",
                        };
                            
const char *Sum[]     = {   "Вычислим производную суммы",  
                            "Самая сложная производная в вашей жизни -- производная суммы!",
                            "Задумайтесь, так ли вы хотите вычислять производную суммы?",
                            "Разделяй и властвуй!",
                        };

const char *Dif[]     = {   "Вычислим производную разности",
                            "Самая сложная производная в вашей жизни -- производная разности!",
                            "Задумайтесь, так ли вы хотите вычислять производную разности?",
                            "Разделяй и властвуй!",
                        };

const char *Div[]     = {   "Вычислим производную частного",
                            "Не все любят производную частного, но придется ее полюбить:",
                            "ТОЛЬКО НЕ ДЕЛЕНИЕ НА 0 ТОЛЬКО НЕ ДЕЛЕНИЕ НА 0",
                        };

const char *Pow[]     = {   "Вычислим производную возведения в степень",
                            "Покажи-ка мне этот показатель, дружок ",
                        };

#define PERSONAL(arr) personal = arr; sizePersonal = SIZEAR(arr); break;

const char *getPrefix(Node *node)
{
    const char **personal = NULL;
    int      sizePersonal = 0;

    switch (node->data.operatorNum)
    {
    case ADD:           PERSONAL(Sum);
    case SUB:           PERSONAL(Dif);
    case DIV:           PERSONAL(Div);
    case POW:           PERSONAL(Pow);
    
    case MUL: case LN:        
    case LOGAR:         personal = NULL;    break;

    default:            personal = NULL;    break;
    }

    int generalSize     = SIZEAR(General);
    int size            = generalSize + sizePersonal;

    int position = abs(rand()) % size;

    if (position < generalSize) return General[position];
    else                        return personal[position - generalSize];
}



#endif //__LATEX_PHRASES_H__