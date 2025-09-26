#include <iostream>
#include "parser.h"

using std::cin;
using std::cout;
using std::endl;
using std::string;

void repl()
{
    string input;
    while (true)
    {
        cout << "glom> ";
        if (!std::getline(cin, input) || input == "exit")
            break;
        try
        {
            auto exprs = parse(input);
            if (exprs.empty())
            {
                continue;
            }
            auto context = Context(nullptr);
            auto result = context.eval(std::move(exprs));
            cout << result->to_string() << endl;
        }catch (std::exception& e)
        {
            cout << "Error: " << e.what() << endl;
        }
    }
}

int main()
{
    cout << "Glom REPL v0.1.0" << endl;
    repl();
    return 0;
}