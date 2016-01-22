/* File: Calculator.cpp
 * -------------------------------------------------------------
 *
 * This programm is a simple calculator, that ask's user to enter line for
 * calculation that looks like "-2*(8/4)^2" and display's the result of this
 * expression. It supports mathematical operations such as "+" - addition,
 * "-" - subtraction, "*" - multiplication, "/" - division and "^" - raising to a power.
 * Any of this operations can be separated by parentheses - "(" and ")"
 */

#include <iostream>
#include <string>
#include <cmath>
#include "console.h"
#include "tokenscanner.h"
#include "simpio.h"
#include "strlib.h"
#include "queueshpp.h"
#include "stackshpp.h"

using namespace std;

/* Function prototypes*/
bool isOperator(string ch);
QueueSHPP<string> toReversePolishNotation(string line);
int opPriority(char op);
double doOperation(double o1, double o2, string op);
double calcLine(QueueSHPP<string> &line);
bool checkParenthesis(string expression);
string filterExpression(string expression);

/* Main programm*/
int main() {

    while (true) {
        string expression = getLine("Enter formula to calculate: ");
        expression = filterExpression(expression);
        if (checkParenthesis(expression)){
            QueueSHPP<string> polishNotation = toReversePolishNotation(expression);
            double result = calcLine(polishNotation);
            cout << "Result: " + expression + " = " << result << endl;
        } else {
            cout << "Wrong expression!!!" << endl;
        }
    }
    return 0;
}

/**
 * Function: filterExpression
 * Usage: expression = filterExpression(expression);
 * -----------------------------------------------------------------------------------------------------
 *
 * This feature filters the incoming formula by removing from it all characters except digits,
 * parentheses, and operators.
 *
 * @param expression String for filtering.
 * @return Received string without unnecessary characters.
 */
string filterExpression(string expression){
    string result;
    for(char ch: expression){
        if (isdigit(ch) || isOperator(string(1,ch)) || ch == '(' || ch == ')' || ch == '.'){
            result += ch;
        }
    }

    return result;
}

/**
 * Function:  checkParenthesis
 * Usage: if (checkParenthesis(expression))...
 * -----------------------------------------------------------------------------------------------------
 *
 * This function checking the balancing of the parentheses in received expression using stack.
 * It add's open parenthesis to stack until meets closing parenthesis. After this it pop parenthesis
 * from the stack and proceed this operation to the end of the expression. If parentheses were left on
 * the stack or in the string, then expression is not correct.
 *
 * @param expression String to check balancing of the parentheses.
 * @return Boolean true if received expression is correct, false if not.
 */
bool checkParenthesis(string expression){
    bool result = true;
    StackSHPP<char> stack;
    for(char ch: expression){
        if(ch == '('){
            stack.push(ch);
        }
        if (ch == ')' && !stack.isEmpty()){
            stack.pop();
        } else if (ch == ')' && stack.isEmpty()) {
            result = false;
        }

    }
    if (!stack.isEmpty()){
        result = false;
    }
    return result;
}


/**
 * Function: toReversePolishNotation
 * Usage: QueueSHPP<string> polishNotation = toReversePolishNotation(expression);
 * -------------------------------------------------------------------------------------------------------
 *
 * This function parsing incoming string line separating it to the operands, operators and parentheses.
 * After this  transform expression to the Reverse Polish notation. In this form of notation operators
 * follows their operand. Function builds RPN in the right order of the operations considering parentheses.
 *
 * @param line Right infix expression with operands, operators and parentheses.
 * @return Received string expression in RPN saved in the queue.
 */
QueueSHPP<string> toReversePolishNotation(string line){
    QueueSHPP<string> result;
    StackSHPP<string> operators; //stack for temporary saviving operators
    TokenScanner scanner(line);
    scanner.addOperator("+");
    scanner.addOperator("-");
    scanner.addOperator("*");
    scanner.addOperator("/");
    scanner.addOperator("^");
    scanner.scanNumbers();
    scanner.ignoreWhitespace();

    bool minusTrigger = false;
    string prevToken = "";

    while(scanner.hasMoreTokens()){
        string token = scanner.nextToken();

        if (isdigit(token[0]) && minusTrigger){
            string tmpStr = "-" + token;

            result.enqueue(tmpStr);
            minusTrigger = false;
        } else if (isdigit(token[0])){
            result.enqueue(token);
        } else if ((token == "-" && prevToken == "") || (token == "-" && prevToken == "(")){ // check for "-" before operand
            minusTrigger = true;

        } else if (token == "(") {
            operators.push(token);
        } else if (token == ")") {
            while (operators.top() != "(" ) {
                result.enqueue(operators.pop());
            }
            operators.pop();
        } else if (isOperator(token)) {
            while (operators.size() > 0 && opPriority(operators.top()[0]) >= opPriority(token[0]) ) { //control right order of the operations
                result.enqueue(operators.pop());
            }
            operators.push(token);
        }
        prevToken = token;
    }
    while (!operators.isEmpty()) {
        result.enqueue(operators.pop());
    }

    return result;
}

/**
 * Function:  calcLine
 * Usage: double result = calcLine(polishNotation);
 * ---------------------------------------------------------------------------------------------------------
 *
 * This function receives the expression in the postfix notation and calculates it's value. It removes numbers
 * from the queue and puts them on the stack until it meets the operator. Then it performs the operation and
 * push the result on the stack and proceed go through the incoming queue. At the end result of the expression
 * is the last element of the stack.
 *
 * @param line Received queue with operators and operands in the Reverse Polish notation.
 * @return Result of the expression.
 */
double calcLine(QueueSHPP<string> &line){
    StackSHPP<double> numbers;

    while (!line.isEmpty()) {
        string tmp = line.dequeue();
        if (tmp.length() > 1 && tmp[0] == '-'){
            numbers.push(-stod(tmp.substr(1)));
        } else if (tmp.length() > 1 && isdigit(tmp[0])){
            numbers.push(stod(tmp));
        } else if (isdigit(tmp[0])){
            numbers.push(stod(tmp));
        } else {
            double o2 = numbers.pop();
            double o1 = numbers.pop();
            numbers.push(doOperation(o1, o2, tmp));
        }
    }
    return numbers.pop();
}

/**
 * Function: isOperator
 * Usage: if (isOperator(token))...
 * ---------------------------------------------------------------------------------
 *
 * Checks whether the incoming character is operator.
 *
 * @param ch Character for checking in the string format.
 * @return Boolean true if operator, false if not.
 */
bool isOperator(string ch) {
    return (ch == "+" || ch == "-" || ch == "/" || ch == "*" || ch == "^");
}

/**
 * Function:  opPriority
 * Usage: while (operators.size() > 0 && opPriority(operators.top()[0]) >= opPriority(token[0]) )...
 * ------------------------------------------------------------------------------------------------
 *
 * This function determines the priority of the received operator by the algebraic rules.
 *
 * @param op Received operator for checking
 * @return Priority of the operator
 */
int opPriority(char op){
    switch (op) {
    case '+':
    case '-':
        return 1;
    case '*':
    case '/':
        return 2;
    case '^':
        return 3;

    }
    return 0;
}

/**
 * Function: doOperation
 * Usage: numbers.push(doOperation(o1, o2, tmp));
 *
 * This function executes the recived operation between two received operands.
 *
 * @param o1 Operand 1
 * @param o2 Operand 2
 * @param op Operator
 * @return Result of the calculation
 */
double doOperation(double o1, double o2, string op){
    double result = 0.0;

    if (op == "+"){
        result = o1 + o2;
    } else if (op == "-") {
        result = o1 - o2;
    } else if (op == "*") {
        result = o1 * o2;
    } else if (op == "/") {
        result = o1 / o2;
    } else if (op == "^") {
        result = pow(o1, o2);
    }
    return result;
}

