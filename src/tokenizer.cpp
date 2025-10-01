//
// Created by glom on 9/25/25.
//

#include "tokenizer.h"


#include <charconv>
#include <memory>
#include <sstream>
#include <variant>

#include "error.h"

using std::string;
using std::variant;
using std::monostate;
using std::unique_ptr;

Token::Token(integer x) : type(TOKEN_NUMBER_INT), value(x)
{
};

Token::Token(rational x) : type(TOKEN_NUMBER_RAT), value(std::move(x))
{
};

Token::Token(real x) : type(TOKEN_NUMBER_REAL), value(x)
{
};

Token::Token(bool x) : type(TOKEN_BOOLEAN), value(x)
{
};

Token::Token(const TokenType token, string x) : type(token), value(std::move(x))
{
};

Token::Token(const TokenType token) : type(token), value(monostate{})
{
};

TokenType Token::get_type() const
{
    return type;
}

integer Token::as_number_int() const
{
    return std::get<integer>(value);
}

rational Token::as_number_rat() const
{
    return std::get<rational>(value);
}

real Token::as_number_real() const
{
    return std::get<real>(value);
}

string& Token::as_string()
{
    return std::get<string>(value);
}

bool Token::as_boolean() const
{
    return std::get<bool>(value);
}

Token Token::make_boolean(const bool x)
{
    return Token(x);
}

Token Token::make_number_int(integer x)
{
    return Token(std::move(x));
}

Token Token::make_number_rat(rational x)
{
    return Token(std::move(x));
}

Token Token::make_number_real(const real x)
{
    return Token(x);
}

Token Token::make_symbol(string x)
{
    return Token{TOKEN_SYMBOL, std::move(x)};
}

Token Token::make_string(string x)
{
    return Token{TOKEN_STRING, std::move(x)};
}

Token Token::make_left_paren()
{
    return Token(TOKEN_LPAREN);
}

Token Token::make_right_paren()
{
    return Token(TOKEN_RPAREN);
}

Token Token::make_quote()
{
    return Token(TOKEN_QUOTE);
}

Token Token::make_end_of_input()
{
    return Token(TOKEN_EOI);
}


Tokenizer::Tokenizer(string input) : input(std::move(input)), index(0)
{
}


Token Tokenizer::next_number()
{
    size_t start = index;
    // Optional sign, std::from_chars doesn't handle leading '+' sign
    if (index < input.size())
    {
        if (input[index] == '-')
            index++;
        else if (input[index] == '+')
        {
            index++;
            start++;
        }
    }

    // Integer part
    while (index < input.size() && std::isdigit(input[index]))
    {
        index++;
    }

    // Decimal part
    if (index >= input.size())
    {
        return Token::make_number_int(integer(input.substr(start, index - start)));
    }

    bool decimal = false;
    if (input[index] == '.')
    {
        decimal = true;
        index++;
        while (index < input.size() && std::isdigit(input[index]) )
        {
            index++;
        }
    }
    if (input[index] == 'e')
    {
        decimal = true;
        index++;
        if (index < input.size() && (input[index] == '+' || input[index] == '-'))
        {
            index++;
        }
        while (index < input.size() && std::isdigit(input[index]))
        {
            index++;
        }
    }
    if (decimal)
    {
        // Convert substring to real
        const string real_str = input.substr(start, index - start);
        try
        {
            return Token::make_number_real(from_string(real_str));
        }
        catch (const std::exception& _)
        {
            throw GlomError("Invalid real number format: " + real_str);
        }
    }

    // Convert substring to integer
    auto num = integer(input.substr(start, index - start));
    // Rational part
    if (index > input.size() || input[index] != '/')
    {
        return Token::make_number_int(std::move(num));
    }
    index++; // Skip '/'
    const size_t denom_start = index;
    while (index < input.size() && std::isdigit(input[index]))
    {
        index++;
    }
    auto den = integer(input.substr(denom_start, index - denom_start));
    if (den.is_zero())
    {
        throw GlomError("Division by zero in rational number");
    }
    return Token::make_number_rat({std::move(num), std::move(den)});
}

Token Tokenizer::next_string()
{
    index++; // Skip "
    bool escaped = false;
    std::stringstream stream;

    while (index < input.size())
    {
        if (escaped)
        {
            switch (input[index])
            {
            case 'n': stream << '\n';
                break;
            case 't': stream << '\t';
                break;
            case 'r': stream << '\r';
                break;
            case '"': stream << '"';
                break;
            case '\\': stream << '\\';
                break;
            default:
                throw GlomError(std::string("Invalid escape sequence: \\") + input[index]);
            }
            escaped = false;
            index++;
            continue;
        }
        switch (const char current = input[index])
        {
        case '\\':
            escaped = true;
            index++;
            continue;
        case '"':
            {
                // End of string
                index++;
                const string str = stream.str();
                return Token::make_string(str);
            }
        default:
            stream << current;
            break;
        }
        index++;
    }
    throw GlomError("Unterminated string literal");
}

Token Tokenizer::next_symbol_or_boolean()
{
    const size_t start = index;
    while (index < input.size()
        && !isspace(input[index])
        && input[index] != '('
        && input[index] != ')')
    {
        index++;
    }
    string ident = input.substr(start, index - start);
    if (ident == "true" || ident == "#t")
    {
        return Token::make_boolean(true);
    }
    if (ident == "false" || ident == "#f")
    {
        return Token::make_boolean(false);
    }
    return Token::make_symbol(std::move(ident));
}

Token Tokenizer::next()
{
    if (index >= input.size())
    {
        return Token::make_end_of_input();
    }
    const char current = input[index];

    if (current == ';')
    {
        // Skip comment until end of line
        while (index < input.size() && input[index] != '\n')
        {
            index++;
        }
        index++;
        return next();
    }

    if (current == '(')
    {
        index++;
        return Token::make_left_paren();
    }
    if (current == ')')
    {
        index++;
        return Token::make_right_paren();
    }
    if (current == '\'')
    {
        index++;
        return Token::make_quote();
    }
    if (isspace(current))
    {
        index++;
        return next();
    }
    if (std::isdigit(current)
        || ((current == '+' || current == '-')
            && index + 1 < input.size() && std::isdigit(input[index + 1])))
    {
        return next_number();
    }
    if (current == '"')
    {
        return next_string();
    }
    return next_symbol_or_boolean();
}
