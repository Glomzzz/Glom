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


Tokenizer::Tokenizer(const string& input): input(input), index(0)
{
}


Token Tokenizer::nextNumber()
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
        return Token(value);
    } catch (const std::exception& _) {
        throw std::runtime_error("Invalid number format: " + numberStr);
    }
}

Token Tokenizer::nextString()
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
                string str = stream.str();
                return {TOKEN_STRING,std::move(str)};
            }
        default:
            stream << current;
            break;
        }
        index++;
    }
    throw std::runtime_error("Unterminated string literal");
}

Token Tokenizer::nextSymbolOrBoolean()
{
    const size_t start = index;
    while (index < input.size()
            && !isspace(input[index])
            && input[index] != '('
            && input[index] != ')'){
        index++;
    }
    string ident = input.substr(start, index - start);
    if (ident == "true" || ident == "#t") {
        return Token(true);
    }
    if (ident == "false"|| ident == "#f") {
        return Token(false);
    }
    return {TOKEN_SYMBOL,std::move(ident)};
}

Token Tokenizer::next()
{
    if (index >= input.size()) {
        return Token(TOKEN_EOI);
    }
    const char current = input[index];

    if (current == '(')
    {
        index++;
        return Token(TOKEN_LPAREN);
    }
    if (current == ')')
    {
        index++;
        return  Token(TOKEN_RPAREN);
    }
    if (current == '\'')
    {
        index++;
        return Token(TOKEN_QUOTE);
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
        return nextNumber();
    }
    if (current == '"')
    {
        return nextString();
    }
    return nextSymbolOrBoolean();
}
