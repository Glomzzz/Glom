#include <iostream>
#include <string>
#include <memory>
#include <sstream>
#include <vector>

#include "parser.h"
#include "context.h"
#include "expr.h"

using namespace std::string_literals;

class MultiLineReader
{
    std::stringstream buffer;
    int line_count = 0;
    std::string prompt = "> ";
    std::string continue_prompt = "... ";

public:
    static bool is_input_complete(const std::string& input)
    {
        if (input.empty()) return false;

        int paren_count = 0;
        bool in_string = false;
        bool escape_next = false;

        for (const char c : input)
        {
            if (escape_next)
            {
                escape_next = false;
                continue;
            }

            if (in_string)
            {
                if (c == '\\')
                {
                    escape_next = true;
                }
                else if (c == '"')
                {
                    in_string = false;
                }
                continue;
            }

            switch (c)
            {
            case '(':
                paren_count++;
                break;
            case ')':
                paren_count--;
                break;
            case '"':
                in_string = true;
                break;
            case ';':
                // Skip comments until end of line
                // Comments don't affect paren balancing
                break;
            default:
                break;
            }
        }

        return paren_count == 0 && !in_string && !escape_next;
    }

    std::string read_input()
    {
        buffer.str("");
        buffer.clear();
        line_count = 0;

        std::string current_line;

        while (true)
        {
            // Display appropriate prompt
            if (line_count == 0)
            {
                std::cout << prompt << std::flush;
            }
            else
            {
                std::cout << continue_prompt << std::flush;
            }

            // Read line with proper error handling
            if (!std::getline(std::cin, current_line))
            {
                if (std::cin.eof())
                {
                    // Ctrl+D pressed
                    if (line_count == 0)
                    {
                        return "exit";
                    }
                    else
                    {
                        // Incomplete input on EOF, treat as exit
                        std::cout << "\n";
                        return "exit";
                    }
                }
                else
                {
                    // Other error
                    throw std::runtime_error("Input error");
                }
            }

            line_count++;

            // Handle commands on first line only
            if (line_count == 1)
            {
                // Trim whitespace for command checking
                std::string trimmed = current_line;
                trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r\f\v"));
                trimmed.erase(trimmed.find_last_not_of(" \t\n\r\f\v") + 1);

                if (trimmed == "exit" || trimmed == "quit")
                {
                    return "exit";
                }
                if (trimmed == "help")
                {
                    std::cout << "Available commands: exit, quit, help, clear\n";
                    std::cout << "Multi-line input is supported. Complete all brackets and quotes.\n";
                    return "";
                }
                if (trimmed == "clear")
                {
                    std::cout << "\033[2J\033[1;1H";
                    return "";
                }
            }

            // Skip empty first lines
            if (line_count == 1 &&
                current_line.find_first_not_of(" \t\n\r\f\v") == std::string::npos)
            {
                return "";
            }

            // Add line to buffer
            if (line_count > 1)
            {
                buffer << "\n";
            }
            buffer << current_line;

            std::string current_input = buffer.str();

            // Check if input is complete
            if (is_input_complete(current_input))
            {
                return current_input;
            }
        }
    }
};

void repl()
{
    MultiLineReader reader;
    const auto context = make_root_context();

    std::cout << "Glom REPL v0.1.0 (Multi-line enabled)\n";
    std::cout << "Type 'exit' to quit, 'help' for help, Ctrl+D to exit\n";

    while (true)
    {
        std::string input;

        try
        {
            input = reader.read_input();
        }
        catch (const std::exception& e)
        {
            std::cerr << "Input error: " << e.what() << '\n';
            break;
        }

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
            if (!exprs || exprs->empty())
            {
                continue;
            }

            const auto result = eval(context, exprs);
            if (result)
            {
                std::cout << result->to_string() << '\n';
            }
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