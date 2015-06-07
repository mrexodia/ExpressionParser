#include <iostream>
#include <string>
#include "expressionparser.h"

int main()
{
    while (true)
    {
        std::cout << "> ";
        std::string expression;
        std::getline(std::cin, expression);
        if (!expression.length())
            break;
        ExpressionParser parser(expression);
        uint value;
        if (!parser.calculate(value))
            std::cout << "Invalid expression!" << std::endl;
        else
            std::cout << "Result: " << value << std::endl;
    }
    return 0;
}