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
            cout << exprs[exprs.size() - 1]->toString() << endl;
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