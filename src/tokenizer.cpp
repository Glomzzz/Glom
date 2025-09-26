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

TokenType Token::getType() const
{
    return type;
}
double Token::asNumber() const
{
    return std::get<double>(value);
}
string& Token::asString()
{
    return std::get<string>(value);
}
bool Token::asBoolean() const
{
    return std::get<bool>(value);
}
Token Token::makeNumber(const double x)
{
    return Token(x);
}
Token Token::makeBoolean(const bool x)
{
    return Token(x);
}
Token Token::makeSymbol( string x)
{
    return Token(TOKEN_SYMBOL, std::move(x));
}
Token Token::makeString( string x)
{
    return Token(TOKEN_STRING, std::move(x));
}
Token Token::makeLParen()
{
    return Token(TOKEN_LPAREN);
}
Token Token::makeRParen()
{
    return Token(TOKEN_RPAREN);
}
Token Token::makeQuote()
{
    return Token(TOKEN_QUOTE);
}
Token Token::makeEOI()
{
    return Token(TOKEN_EOI);
}



Tokenizer::Tokenizer(string input): input(std::move(input)), index(0){}


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
        return Token::makeNumber(value);
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
                const string str = stream.str();
                return Token::makeString(str);
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
    const string ident = input.substr(start, index - start);
    if (ident == "true" || ident == "#t") {
        return Token::makeBoolean(true);
    }
    if (ident == "false"|| ident == "#f") {
        return Token::makeBoolean(false);
    }
    return Token::makeSymbol(ident);
}

Token Tokenizer::next()
{
    if (index >= input.size()) {
        return Token::makeEOI();
    }
    const char current = input[index];

    if (current == '(')
    {
        index++;
        return Token::makeLParen();
    }
    if (current == ')')
    {
        index++;
        return  Token::makeRParen();
    }
    if (current == '\'')
    {
        index++;
        return Token::makeQuote();
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
