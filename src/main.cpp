#include <iostream>

#include "parser.h"
#include "context.h"
#include "expr.h"

using std::cin;
using std::cout;
using std::endl;
using std::string;

void repl()
{
    string input;
    const auto context = make_root_context();
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
            const auto result = context->eval(exprs);
            cout << result->to_string() << endl;
        }
        catch (const std::exception &e)
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