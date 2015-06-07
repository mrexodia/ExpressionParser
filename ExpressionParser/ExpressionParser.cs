using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ExpressionParser
{
    class ExpressionParser
    {
        class Token
        {
            public enum Type
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
            }

            public enum Associativity
            {
                LeftToRight,
                RightToLeft,
                Unspecified
            }

            public override string ToString()
            {
                return string.Format("[data=\"{0}\", type={1}]", data, type);
            }

            public string data { get; private set; }
            public Type type { get; private set; }

            public Token(string data, Type type)
            {
                this.data = data;
                this.type = type;
            }

            public Associativity associativity
            {
                get
                {
                    switch (type)
                    {
                        case Type.OperatorUnarySub:
                        case Type.OperatorNot:
                            return Associativity.RightToLeft;
                        case Type.OperatorMul:
                        case Type.OperatorHiMul:
                        case Type.OperatorDiv:
                        case Type.OperatorMod:
                        case Type.OperatorAdd:
                        case Type.OperatorSub:
                        case Type.OperatorShl:
                        case Type.OperatorShr:
                        case Type.OperatorAnd:
                        case Type.OperatorXor:
                        case Type.OperatorOr:
                            return Associativity.LeftToRight;
                        default:
                            return Associativity.Unspecified;
                    }
                }
            }

            public int precedence
            {
                get
                {
                    switch (type)
                    {
                        case Type.OperatorUnarySub:
                        case Type.OperatorNot:
                            return 7;
                        case Type.OperatorMul:
                        case Type.OperatorHiMul:
                        case Type.OperatorDiv:
                        case Type.OperatorMod:
                            return 6;
                        case Type.OperatorAdd:
                        case Type.OperatorSub:
                            return 5;
                        case Type.OperatorShl:
                        case Type.OperatorShr:
                            return 4;
                        case Type.OperatorAnd:
                            return 3;
                        case Type.OperatorXor:
                            return 2;
                        case Type.OperatorOr:
                            return 1;
                        default:
                            return 0;
                    }
                }
            }

            public bool isOperator
            {
                get
                {
                    return type != Type.Data && type != Type.OpenBracket && type != Type.CloseBracket;
                }
            }
        }

        private List<Token> _tokens = new List<Token>();
        private Token[] _prefixTokens = null;
        private string _curToken = "";

        public ExpressionParser(string expression)
        {
            tokenize(expression);
            shuntingYard();
        }

        private bool isUnaryOperator()
        {
            if (_curToken.Length != 0)
                return false;
            if (_tokens.Count == 0)
                return true;
            var lastToken = _tokens[_tokens.Count - 1];
            return lastToken.isOperator;
        }

        private void tokenize(string expression)
        {
            foreach (var ch in expression)
            {
                switch (ch)
                {
                    case '(':
                        addOperatorToken(ch, Token.Type.OpenBracket);
                        break;
                    case ')':
                        addOperatorToken(ch, Token.Type.CloseBracket);
                        break;
                    case '~':
                        addOperatorToken(ch, Token.Type.OperatorNot);
                        break;
                    case '*':
                        addOperatorToken(ch, Token.Type.OperatorMul);
                        break;
                    case '`':
                        addOperatorToken(ch, Token.Type.OperatorHiMul);
                        break;
                    case '/':
                        addOperatorToken(ch, Token.Type.OperatorDiv);
                        break;
                    case '%':
                        addOperatorToken(ch, Token.Type.OperatorMod);
                        break;
                    case '+':
                        if (!isUnaryOperator()) //skip all unary add operators
                            addOperatorToken(ch, Token.Type.OperatorAdd);
                        break;
                    case '-':
                        if (isUnaryOperator())
                            addOperatorToken(ch, Token.Type.OperatorUnarySub);
                        else
                            addOperatorToken(ch, Token.Type.OperatorSub);
                        break;
                    case '<':
                        addOperatorToken(ch, Token.Type.OperatorShl);
                        break;
                    case '>':
                        addOperatorToken(ch, Token.Type.OperatorShr);
                        break;
                    case '&':
                        addOperatorToken(ch, Token.Type.OperatorAnd);
                        break;
                    case '^':
                        addOperatorToken(ch, Token.Type.OperatorXor);
                        break;
                    case '|':
                        addOperatorToken(ch, Token.Type.OperatorOr);
                        break;
                    case ' ': //ignore spaces
                        break;
                    default:
                        _curToken += ch;
                        break;
                }
            }
            if (_curToken.Length != 0)
                _tokens.Add(new Token(_curToken, Token.Type.Data));
        }

        private bool shuntingYard()
        {
            //Implementation of Dijkstra's Shunting-yard algorithm
            var queue = new Queue<Token>(_tokens.Count);
            var stack = new Stack<Token>(_tokens.Count);
            foreach (var token in _tokens)
            {
                switch (token.type)
                {
                    case Token.Type.Data:
                        queue.Enqueue(token);
                        break;
                    case Token.Type.OpenBracket:
                        stack.Push(token);
                        break;
                    case Token.Type.CloseBracket:
                        while (true)
                        {
                            if (stack.Count == 0) //empty stack = bracket mismatch
                                return false;
                            var curToken = stack.Pop();
                            if (curToken.type == Token.Type.OpenBracket)
                                break;
                            queue.Enqueue(curToken);
                        }
                        break;
                    default: //operator
                        var o1 = token;
                        while (stack.Count != 0)
                        {
                            var o2 = stack.Peek();
                            if (o2.isOperator &&
                                (o1.associativity == Token.Associativity.LeftToRight && o1.precedence <= o2.precedence) ||
                                (o1.associativity == Token.Associativity.RightToLeft && o1.precedence < o2.precedence))
                            {
                                queue.Enqueue(stack.Pop());
                            }
                            else
                                break;
                        }
                        stack.Push(o1);
                        break;
                }
            }
            while (stack.Count != 0)
            {
                var curToken = stack.Pop();
                if (curToken.type == Token.Type.OpenBracket || curToken.type == Token.Type.CloseBracket)
                    return false;
                queue.Enqueue(curToken);
            }
            _prefixTokens = queue.ToArray();
            return true;
        }

        private void addOperatorToken(char ch, Token.Type type)
        {
            if (_curToken.Length > 0)
            {
                _tokens.Add(new Token(_curToken, Token.Type.Data));
                _curToken = "";
            }
            _tokens.Add(new Token(ch.ToString(), type));
        }

        public void Print()
        {
            if (_prefixTokens == null)
                return;
            foreach (var token in _prefixTokens)
                Console.WriteLine(token);
        }

        private bool operation(Token.Type type, uint op1, uint op2, out uint result)
        {
            result = 0;
            switch (type)
            {
                case Token.Type.OperatorUnarySub:
                    result = (uint)-(int)op1;
                    return true;
                case Token.Type.OperatorNot:
                    result = ~op1;
                    return true;
                case Token.Type.OperatorMul:
                    result = op1 * op2;
                    return true;
                case Token.Type.OperatorHiMul:
                    result = op1 * op2;
                    return true;
                case Token.Type.OperatorDiv:
                    if (op2 != 0)
                    {
                        result = op1 / op2;
                        return true;
                    }
                    return false;
                case Token.Type.OperatorMod:
                    if (op2 != 0)
                    {
                        result = op1 % op2;
                        return true;
                    }
                    return false;
                case Token.Type.OperatorAdd:
                    result = op1 + op2;
                    return true;
                case Token.Type.OperatorSub:
                    result = op1 - op2;
                    return true;
                case Token.Type.OperatorShl:
                    result = op1 << (int)op2;
                    return true;
                case Token.Type.OperatorShr:
                    result = op1 >> (int)op2;
                    return true;
                case Token.Type.OperatorAnd:
                    result = op1 & op2;
                    return true;
                case Token.Type.OperatorXor:
                    result = op1 ^ op2;
                    return true;
                case Token.Type.OperatorOr:
                    result = op1 | op2;
                    return true;
                default:
                    return false;
            }
        }

        private bool valFromString(string data, out uint value)
        {
            return uint.TryParse(data, out value);
        }

        public bool Calculate(out uint value)
        {
            value = 0;
            if (_prefixTokens == null)
                return false;
            var stack = new Stack<uint>(_prefixTokens.Length);
            foreach (var token in _prefixTokens)
            {
                if (token.isOperator)
                {
                    uint op1 = 0;
                    uint op2 = 0;
                    uint result = 0;
                    switch (token.type)
                    {
                        case Token.Type.OperatorUnarySub:
                        case Token.Type.OperatorNot:
                            if (stack.Count < 1)
                                return false;
                            op1 = stack.Pop();
                            operation(token.type, op1, op2, out result);
                            stack.Push(result);
                            break;
                        case Token.Type.OperatorMul:
                        case Token.Type.OperatorHiMul:
                        case Token.Type.OperatorDiv:
                        case Token.Type.OperatorMod:
                        case Token.Type.OperatorAdd:
                        case Token.Type.OperatorSub:
                        case Token.Type.OperatorShl:
                        case Token.Type.OperatorShr:
                        case Token.Type.OperatorAnd:
                        case Token.Type.OperatorXor:
                        case Token.Type.OperatorOr:
                            if (stack.Count < 2)
                                return false;
                            op2 = stack.Pop();
                            op1 = stack.Pop();
                            operation(token.type, op1, op2, out result);
                            stack.Push(result);
                            break;
                        default: //do nothing
                            break;
                    }
                }
                else
                {
                    uint result;
                    if (!valFromString(token.data, out result))
                        return false;
                    stack.Push(result);
                }

            }
            if (stack.Count == 0)
                return false;
            value = stack.Pop();
            return true;
        }
    }
}
