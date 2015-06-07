using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ExpressionParser
{
    class Program
    {
        static void Main(string[] args)
        {
            while (true)
            {
                Console.Write("> ");
                string expression = Console.ReadLine();
                if (expression.Length == 0)
                    break;
                var parser = new ExpressionParser(expression);
                uint value;
                if (!parser.Calculate(out value))
                    Console.WriteLine("Invalid expression!");
                else
                    Console.WriteLine("Result: {0}", (int)value);
            }
        }
    }
}