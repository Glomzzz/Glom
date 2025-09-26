//
// Created by glom on 9/25/25.
//

#include "tokenizer.h"


#include <iostream>
#include <memory>
#include <sstream>
#include <variant>

using std::string;
using std::variant;
using std::monostate;
using std::unique_ptr;

Token::Token(double x) : type(TOKEN_NUMBER), value(x) {};
Token::Token(bool x) : type(TOKEN_BOOLEAN), value(x) {};
Token::Token(const TokenType token, string x) : type(token), value(std::move(x)) {};
Token::Token(const TokenType token): type(token), value(monostate{}) {};

TokenType Token::get_type() const
{
    return type;
}
double Token::as_number() const
{
    return std::get<double>(value);
}
string& Token::as_string()
{
    return std::get<string>(value);
}
bool Token::as_boolean() const
{
    return std::get<bool>(value);
}
Token Token::make_number(const double x)
{
    return Token(x);
}
Token Token::make_boolean(const bool x)
{
    return Token(x);
}
Token Token::make_symbol( string x)
{
    return Token(TOKEN_SYMBOL, std::move(x));
}
Token Token::make_string( string x)
{
    return Token(TOKEN_STRING, std::move(x));
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



Tokenizer::Tokenizer(string input): input(std::move(input)), index(0){}


Token Tokenizer::next_number()
{
    const size_t start = index;

    // Optional sign
    if (index < input.size() && (input[index] == '+' || input[index] == '-')) {
        index++;
    }

    // Integer part
    while (index < input.size() && std::isdigit(input[index])) {
        index++;
    }

    // Decimal part
    if (index < input.size() && input[index] == '.') {
        index++;
        while (index < input.size() && std::isdigit(input[index])) {
            index++;
        }
    }

    // Convert substring to double
    const string numberStr = input.substr(start, index - start);
    try {
        const double value = std::stod(numberStr);
        return Token::make_number(value);
    } catch (const std::exception& _) {
        throw std::runtime_error("Invalid number format: " + numberStr);
    }
}

Token Tokenizer::next_string()
{
    index++; // Skip "
    bool escaped = false;
    std::stringstream stream;

    while (index < input.size()) {
        if (escaped)
        {
            switch (input[index]) {
                case 'n': stream << '\n'; break;
                case 't': stream << '\t'; break;
                case 'r': stream << '\r'; break;
                case '"': stream << '"'; break;
                case '\\': stream << '\\'; break;
                default:
                    throw std::runtime_error(std::string("Invalid escape sequence: \\") + input[index]);
            }
            escaped = false;
            index++;
            continue;
        }
        switch (const char current = input[index]) {
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
    throw std::runtime_error("Unterminated string literal");
}

Token Tokenizer::next_symbol_or_boolean()
{
    const size_t start = index;
    while (index < input.size()
            && !isspace(input[index])
            && input[index] != '('
            && input[index] != ')'){
        index++;
    }
    const string ident = input.substr(start, index - start);
    if (ident == "true" || ident == "#t") {
        return Token::make_boolean(true);
    }
    if (ident == "false"|| ident == "#f") {
        return Token::make_boolean(false);
    }
    return Token::make_symbol(ident);
}

Token Tokenizer::next()
{
    if (index >= input.size()) {
        return Token::make_end_of_input();
    }
    const char current = input[index];

    if (current == ';')
    {
        // Skip comment until end of line
        while (index < input.size() && input[index] != '\n') {
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
        return  Token::make_right_paren();
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
