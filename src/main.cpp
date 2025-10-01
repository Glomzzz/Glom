#include <iostream>
#include <string>
#include <memory>
#include <sstream>

#include "parser.h"
#include "context.h"
#include "expr.h"

using namespace std::string_literals;

class MultiLineReader
{
    std::stringstream buffer;
    int line_count = 0;
    std::string prompt = "> ";

public:
    static bool is_input_complete(const std::string& input)
    {
        if (input.empty()) return false;

        int paren_count = 0;
        bool str = false;
        bool escape = false;
        for (const char c : input)
        {
            if (str)
            {
                if (escape)
                {
                    escape = false;
                }
                else if (c == '\\')
                {
                    escape = true;
                }
                else if (c == '"')
                {
                    str = false;
                }
                continue;
            }
            switch (c)
            {
            case '(': paren_count++;
                break;
            case ')': paren_count--;
                break;
            case '"': str = true;
            default: ;
            }
        }

        if (paren_count != 0)
        {
            return false;
        }


        return true;
    }

    std::string read_input()
    {
        buffer.str("");
        buffer.clear();
        line_count = 0;

        std::string current_line;

        while (true)
        {
            if (line_count == 0)
            {
                std::cout << prompt << std::flush;
            }
            if (!(std::cin >> current_line))
            {
                // Ctrl+D
                if (line_count == 0)
                {
                    return "exit";
                }
                break;
            }

            line_count++;

            if (line_count == 1)
            {
                if (current_line == "exit" || current_line == "quit")
                {
                    return "exit";
                }
                if (current_line == "help")
                {
                    std::cout << "Available commands: exit, quit, help, clear\n";
                    std::cout << "Multi-line input is supported. End with ';' or complete all brackets.\n";
                    return "";
                }
                if (current_line == "clear")
                {
                    std::cout << "\033[2J\033[1;1H";
                    return "";
                }
            }

            if (line_count == 1 &&
                (current_line.empty() || current_line.find_first_not_of(" \t\n\r") == std::string::npos))
            {
                return "";
            }

            if (line_count > 1)
            {
                buffer << "\n";
            }
            buffer << current_line;
            if (is_input_complete(buffer.str()))
            {
                return buffer.str();
            }
        }
        return buffer.str();
    }
};

void repl()
{
    MultiLineReader reader;
    const auto context = make_root_context();

    std::cout << "Glom REPL v0.1.0 (Multi-line enabled)\n";
    std::cout << "Type 'exit' to quit, 'help' for help\n";

    while (true)
    {
        std::string input = reader.read_input();

        if (input == "exit")
        {
            break;
        }
        if (input.empty())
        {
            continue;
        }

        try
        {
            auto exprs = parse(input);
            if (exprs.empty())
            {
                continue;
            }

            const auto result = context->eval(exprs);
            std::cout << result->to_string() << '\n';
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error: " << e.what() << '\n';
        }
        catch (...)
        {
            std::cerr << "Unknown error occurred\n";
        }
    }

    std::cout << "Goodbye!\n";
}

int main()
{
    try
    {
        repl();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Fatal error: " << e.what() << '\n';
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown fatal error occurred\n";
        return 1;
    }

    return 0;
}
