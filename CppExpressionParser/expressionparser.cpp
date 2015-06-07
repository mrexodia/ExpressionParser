#include "expressionparser.h"
#include <stdio.h>

ExpressionParser::Token::Token(const std::string data, const Type type)
{
    _data = data;
    _type = type;
}

const std::string ExpressionParser::Token::data() const
{
    return _data;
}

const ExpressionParser::Token::Type ExpressionParser::Token::type() const
{
    return _type;
}

const ExpressionParser::Token::Associativity ExpressionParser::Token::associativity() const
{
    switch (_type)
    {
    case Type::OperatorUnarySub:
    case Type::OperatorNot:
        return Associativity::RightToLeft;
    case Type::OperatorMul:
    case Type::OperatorHiMul:
    case Type::OperatorDiv:
    case Type::OperatorMod:
    case Type::OperatorAdd:
    case Type::OperatorSub:
    case Type::OperatorShl:
    case Type::OperatorShr:
    case Type::OperatorAnd:
    case Type::OperatorXor:
    case Type::OperatorOr:
        return Associativity::LeftToRight;
    default:
        return Associativity::Unspecified;
    }
}

const int ExpressionParser::Token::precedence() const
{
    switch (_type)
    {
    case Type::OperatorUnarySub:
    case Type::OperatorNot:
        return 7;
    case Type::OperatorMul:
    case Type::OperatorHiMul:
    case Type::OperatorDiv:
    case Type::OperatorMod:
        return 6;
    case Type::OperatorAdd:
    case Type::OperatorSub:
        return 5;
    case Type::OperatorShl:
    case Type::OperatorShr:
        return 4;
    case Type::OperatorAnd:
        return 3;
    case Type::OperatorXor:
        return 2;
    case Type::OperatorOr:
        return 1;
    default:
        return 0;
    }
}

const bool ExpressionParser::Token::isOperator() const
{
    return _type != Type::Data && _type != Type::OpenBracket && _type != Type::CloseBracket;
}

ExpressionParser::ExpressionParser(const std::string expression)
{
    tokenize(fixClosingBrackets(expression));
    shuntingYard();
}

std::string ExpressionParser::fixClosingBrackets(const std::string expression)
{
    int open = 0;
    int close = 0;
    int len = expression.length();
    for (int i = 0; i < len; i++)
    {
        if (expression[i] == '(')
            open++;
        else if (expression[i] == ')')
            close++;
    }
    std::string result = expression;
    if (close < open)
    {
        for (int i = 0; i < open - close; i++)
            result += ")";
    }
    return result;
}

void ExpressionParser::tokenize(const std::string expression)
{
    int len = expression.length();
    for (int i = 0; i < len; i++)
    {
        char ch = expression[i];
        switch (ch)
        {
        case '(':
            addOperatorToken(ch, Token::Type::OpenBracket);
            break;
        case ')':
            addOperatorToken(ch, Token::Type::CloseBracket);
            break;
        case '~':
            addOperatorToken(ch, Token::Type::OperatorNot);
            break;
        case '*':
            addOperatorToken(ch, Token::Type::OperatorMul);
            break;
        case '`':
            addOperatorToken(ch, Token::Type::OperatorHiMul);
            break;
        case '/':
            addOperatorToken(ch, Token::Type::OperatorDiv);
            break;
        case '%':
            addOperatorToken(ch, Token::Type::OperatorMod);
            break;
        case '+':
            if (!isUnaryOperator()) //skip all unary add operators
                addOperatorToken(ch, Token::Type::OperatorAdd);
            break;
        case '-':
            if (isUnaryOperator())
                addOperatorToken(ch, Token::Type::OperatorUnarySub);
            else
                addOperatorToken(ch, Token::Type::OperatorSub);
            break;
        case '<':
            addOperatorToken(ch, Token::Type::OperatorShl);
            break;
        case '>':
            addOperatorToken(ch, Token::Type::OperatorShr);
            break;
        case '&':
            addOperatorToken(ch, Token::Type::OperatorAnd);
            break;
        case '^':
            addOperatorToken(ch, Token::Type::OperatorXor);
            break;
        case '|':
            addOperatorToken(ch, Token::Type::OperatorOr);
            break;
        case ' ': //ignore spaces
            break;
        default:
            _curToken += ch;
            break;
        }
    }
    if (_curToken.length() != 0)
        _tokens.push_back(Token(_curToken, Token::Type::Data));
}

void ExpressionParser::addOperatorToken(const char ch, const Token::Type type)
{
    if (_curToken.length())
    {
        _tokens.push_back(Token(_curToken, Token::Type::Data));
        _curToken = "";
    }
    std::string data;
    data += ch;
    _tokens.push_back(Token(data, type));
}

bool ExpressionParser::isUnaryOperator()
{
    if (_curToken.length())
        return false;
    if (!_tokens.size())
        return true;
    Token lastToken = _tokens[_tokens.size() - 1];
    return lastToken.isOperator();
}

void ExpressionParser::shuntingYard()
{
    //Implementation of Dijkstra's Shunting-yard algorithm
    std::vector<Token> queue;
    std::stack<Token> stack;
    size_t len = _tokens.size();
    for (size_t i = 0; i < len; i++)
    {
        Token & token = _tokens[i];
        switch (token.type())
        {
        case Token::Type::Data:
            queue.push_back(token);
            break;
        case Token::Type::OpenBracket:
            stack.push(token);
            break;
        case Token::Type::CloseBracket:
            while (true)
            {
                if (stack.empty()) //empty stack = bracket mismatch
                    return;
                Token curToken = stack.top();
                stack.pop();
                if (curToken.type() == Token::Type::OpenBracket)
                    break;
                queue.push_back(curToken);
            }
            break;
        default: //operator
            Token & o1 = token;
            while (!stack.empty())
            {
                Token o2 = stack.top();
                if (o2.isOperator() &&
                    (o1.associativity() == Token::Associativity::LeftToRight && o1.precedence() <= o2.precedence()) ||
                    (o1.associativity() == Token::Associativity::RightToLeft && o1.precedence() < o2.precedence()))
                {
                    queue.push_back(o2);
                    stack.pop();
                }
                else
                    break;
            }
            stack.push(o1);
            break;
        }
    }
    while (!stack.empty())
    {
        Token curToken = stack.top();
        stack.pop();
        if (curToken.type() == Token::Type::OpenBracket || curToken.type() == Token::Type::CloseBracket)
            return;
        queue.push_back(curToken);
    }
    _prefixTokens = queue;
}

bool ExpressionParser::operation(const Token::Type type, const uint op1, const uint op2, uint & result)
{
    result = 0;
    switch (type)
    {
    case Token::Type::OperatorUnarySub:
        result = op1 * ~0;
        return true;
    case Token::Type::OperatorNot:
        result = ~op1;
        return true;
    case Token::Type::OperatorMul:
        result = op1 * op2;
        return true;
    case Token::Type::OperatorHiMul:
        result = op1 * op2;
        return true;
    case Token::Type::OperatorDiv:
        if (op2 != 0)
        {
            result = op1 / op2;
            return true;
        }
        return false;
    case Token::Type::OperatorMod:
        if (op2 != 0)
        {
            result = op1 % op2;
            return true;
        }
        return false;
    case Token::Type::OperatorAdd:
        result = op1 + op2;
        return true;
    case Token::Type::OperatorSub:
        result = op1 - op2;
        return true;
    case Token::Type::OperatorShl:
        result = op1 << op2;
        return true;
    case Token::Type::OperatorShr:
        result = op1 >> op2;
        return true;
    case Token::Type::OperatorAnd:
        result = op1 & op2;
        return true;
    case Token::Type::OperatorXor:
        result = op1 ^ op2;
        return true;
    case Token::Type::OperatorOr:
        result = op1 | op2;
        return true;
    default:
        return false;
    }
}

bool ExpressionParser::valFromString(const std::string data, uint & value)
{
    return sscanf_s(data.c_str(), "%u", &value) == 1;
}

bool ExpressionParser::calculate(uint & value)
{
    value = 0;
    if (!_prefixTokens.size())
        return false;
    std::stack<uint> stack;
    size_t len = _prefixTokens.size();
    for (size_t i = 0; i < len; i++)
    {
        Token & token = _prefixTokens[i];
        if (token.isOperator())
        {
            uint op1 = 0;
            uint op2 = 0;
            uint result = 0;
            switch (token.type())
            {
            case Token::Type::OperatorUnarySub:
            case Token::Type::OperatorNot:
                if (stack.size() < 1)
                    return false;
                op1 = stack.top();
                stack.pop();
                operation(token.type(), op1, op2, result);
                stack.push(result);
                break;
            case Token::Type::OperatorMul:
            case Token::Type::OperatorHiMul:
            case Token::Type::OperatorDiv:
            case Token::Type::OperatorMod:
            case Token::Type::OperatorAdd:
            case Token::Type::OperatorSub:
            case Token::Type::OperatorShl:
            case Token::Type::OperatorShr:
            case Token::Type::OperatorAnd:
            case Token::Type::OperatorXor:
            case Token::Type::OperatorOr:
                if (stack.size() < 2)
                    return false;
                op2 = stack.top();
                stack.pop();
                op1 = stack.top();
                stack.pop();
                operation(token.type(), op1, op2, result);
                stack.push(result);
                break;
            default: //do nothing
                break;
            }
        }
        else
        {
            uint result;
            if (!valFromString(token.data(), result))
                return false;
            stack.push(result);
        }

    }
    if (stack.empty())
        return false;
    value = stack.top();
    return true;
}