#ifndef  __LATEX_PHRASES_H__
#define  __LATEX_PHRASES_H__

#include <stdlib.h>
#include <stdio.h>
#include"tree_of_expressions.h"

#define SIZEAR(array) sizeof(array) / sizeof(array[0])

const char *getPrefix(Node *node);

const char *Introduction = 
    "\\documentclass{article}\n\\usepackage[russian]{babel}\n\\usepackage{graphicx}\n\n\\title{Вычисление производной}\n\\author{Авторы статьи возжелали остаться в тени}\n\n\n\\begin{document}\n\n\\maketitle\n\n\\begin{abstract}\n\tВремя - иллюзия. Бот бессмертен. Математика вечна. Аминь\n\\end{abstract}\n\n\\section{Вступление}\n\nУ современного человека в жизни часто возникает потребность дифференцировать. Бывает, что человек не может даже сопротивляться этому порыву: дай продифференцировать что-то или умру! Главная проблема заключается в том, что не каждый человек от рождения умеет вычислять производные функций. Авторы статьи решили помочь обездоленным и предоставить пример дифференцирования одной из функций, с помощью которого любой, даже самый отпетый гуманитарий, смог бы научиться брать простенькие производные.\n\nКроме того, данная статья поясняет одно из важных приложений производных -- исследование функции. Вычисление производных является необходимой и важнейшей задачей при исследовании функции и построении её графика. При известных первой и второй производных читатель с легкостью сможет построить график функции, а данная статья научить эти производные искать.\n\nНесмотря на свои добрые побуждения, авторы статьи не могут знать, к чему приведет ее использование. Авторы статьи не несут ответсвенности за последствия. Всё на вашей совести.\n\n\\section{Вычисление первой производной}\n\n";


const char *Simplification = 
   "Очевидно, что на данное выражение без слез не взглянешь. Авторы считают, что его необходимо упростить. Путем тривиальных преобразований получим:\n\n$f\'(x) = $ ";

const char *Deriv2 = 
    "\n\n\\section{Вычисление второй производной}\n\nЛегко понять, что вычисление второй производной -- дело не менее важное, чем вычисление первой. По определению вторая производная суть производная производной, исходя из чего некоторые авторы статьи посчитали, что читатель, дошедший до этого места, без труда вычислит вторую производную самостоятельно. Всё же, в ходе всенародного голосования единогласно было принято решение привести в данной статье нахождение второй производной заданной функции.\n\n";

const char *Simplification2 = 
    "С помощью нехитрых преобразований можем привести вторую производную к виду:\n\n$f\'\'(x) = $ ";

const char *Taylor = 
    "\\section{Размышления об исследовании функции}\n\nТеперь, когда вычислены первая и вторая производная, дальнейшие действия по исследованию функции тривиальны. Авторы статьи не считают необходимым приводить график заданной функции, ведь мудрый и увлеченный читатель уже сам давно его построил. Для тех же читателей, что еще не постоили график, эта задача предлагается в качестве упражнения.\n\nДля закрепления знаний, связанных с нахождением производной, полученных в данной статье, авторы статьи предлагают читателю вычислить первую, вторую, третью, четвертую и пятую производные в точке $x_0$ в качестве упражнения.\n\nЗная эти производные, можно получить разложение функции по формуле Тейлора в окрестности данной точки. Авторы статьи надеятся, что данное упражнение не вызовет у читателя затруднений.\n\nФормула Тейлора имеет следующий вид:\n$$f(x) = f(x_0) + \\frac{f'(x_0)}{1!}(x - x_0) + \\cdots + \\frac{f^{(n)}(x_0)}{n!}(x - x_0)^n + o((x - x_0)^n).$$\n\nЧтобы упростить читателю задачу, авторы статьи вычислили первую и вторую производную в некоторой точке $x_0$. ";

const char *Deriv3 = 
    "Один из авторов статьи оказался очень сердобольным человеком и настоял на том, чтобы хотя бы третья производная присутствовала в статье в неоктором виде. Авторы статьи решили привести численное значениие этой производной в точке $x_0$ и ничего более, дабы не сокращать интерес читателя к дифференцированию и его любопытство. Читатель! Вполне возможно, что мы допустили ошибку. Надежда только на тебя!\n\n";

const char *Taylor2 = 
    "\\section{Разложение по формуле Тейлора и графики}\n\nТеперь мы можем, ради затравочки, начертить простенький эскиз графика $f(x)$ и отметить на нем разложения по Тейлору вплоть до 3 степени. Читаель сам с легкостью, ориентируясь на эти примеры, начертит точный график функции и отметит разложение до 4 и 5 степеней. Ведь самое сложное в исследовании функции, а именно вычисление первой и второй производной, уже давно сделано, и с оставшейся задачей справится любой пятиклассник.\n\n";

const char *NotDefined = 
    "К сожалению, или функция, или какая-то из ее производных не определены в точке 0, поэтому авторы статьи не могут привести разложение в окрестности этой точки. Но предыдущая часть статьи уже написана, и авторы не в силах это изменить. Читателю предлагается выбрать любую точку, в окрестности которой определены функция и ее производные вплоть до пятого порядка и получить разложение по формуле Тейлора без помощи авторов. Такова суровость жизни: мы не всегда получаем помощь от тех, от кого ее ждём.\n\n";

const char *Conclusion =
    "\\section{Заключение}\n\nИтак, подошла к конца наша общая и полная статья по дифференцированию и исследованию функции. Авторы статьи искренне благодарят за прочтение дошедших до этого момента и желают удачи в дифференцировании. Помните: дифференцируйте свои проблемы, а не интегрируйте! И да пребудет с вами математика\n\n";

const char *References = 
    "\\section*{Источники}\n\\begin{enumerate}\n\t\\item Любовь к математике;\n\t\\item Горящие сердца авторов статьи;\n\t\\item Божественные знания, данные нам Матом и Аном.\n\\end{enumerate}\n\n\\end{document}\n\n";


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
        case SIN: case COS:  
        case L_BRACKET:
        case R_BRACKET:
        case NOT_OPER:      
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