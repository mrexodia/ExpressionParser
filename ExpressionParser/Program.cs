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
                try
                {
                    Console.Write("> ");
                    var parser = new ExpressionParser(Console.ReadLine());
                    uint value;
                    if (!parser.Calculate(out value))
                        Console.WriteLine("Invalid expression!");
                    else
                        Console.WriteLine("Result: {0}", (int)value);
                }
                catch (Exception e)
                {
                    Console.WriteLine(e);
                }
            }
        }
    }
}