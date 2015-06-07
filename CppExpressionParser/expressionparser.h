#ifndef _EXPRESSION_PARSER_H
#define _EXPRESSION_PARSER_H

#include <stack>
#include <vector>
#include <string>

typedef unsigned int uint;

class ExpressionParser
{
public:
    ExpressionParser(std::string expression);
    bool calculate(uint & value);

private:
    class Token
    {
    public:
        enum class Type
        {
            Data,
            OpenBracket,
            CloseBracket,
            OperatorUnarySub,
            OperatorNot,
            OperatorMul,
            OperatorHiMul,
            OperatorDiv,
            OperatorMod,
            OperatorAdd,
            OperatorSub,
            OperatorShl,
            OperatorShr,
            OperatorAnd,
            OperatorXor,
            OperatorOr
        };

        enum class Associativity
        {
            LeftToRight,
            RightToLeft,
            Unspecified
        };

        Token(std::string data, Type type);
        const std::string data();
        const Type type();
        const Associativity associativity();
        const int precedence();
        const bool isOperator();
        
    private:
        std::string _data;
        Type _type;
    };

    std::string fixClosingBrackets(std::string expression);
    bool isUnaryOperator();
    void tokenize(std::string expression);
    bool shuntingYard();
    void addOperatorToken(char ch, Token::Type type);
    bool operation(Token::Type type, uint op1, uint op2, uint & result);
    bool valFromString(std::string data, uint & value);

    std::vector<Token> _tokens;
    std::vector<Token> _prefixTokens;
    std::string _curToken;
};

#endif //_EXPRESSION_PARSER_H