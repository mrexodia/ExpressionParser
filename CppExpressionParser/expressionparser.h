#ifndef _EXPRESSION_PARSER_H
#define _EXPRESSION_PARSER_H

#include <stack>
#include <vector>
#include <string>

typedef unsigned int uint;

class ExpressionParser
{
public:
    ExpressionParser(const std::string & expression);
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

        Token(const std::string & data, const Type type);
        const std::string data() const;
        const Type type() const;
        const Associativity associativity() const;
        const int precedence() const;
        const bool isOperator() const;
        
    private:
        std::string _data;
        Type _type;
    };

    std::string fixClosingBrackets(const std::string & expression);
    bool isUnaryOperator();
    void tokenize(const std::string & expression);
    void shuntingYard();
    void addOperatorToken(const char ch, const Token::Type type);
    bool operation(const Token::Type type, const uint op1, const uint op2, uint & result);
    bool valFromString(const std::string & data, uint & value);

    std::vector<Token> _tokens;
    std::vector<Token> _prefixTokens;
    std::string _curToken;
};

#endif //_EXPRESSION_PARSER_H