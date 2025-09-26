//
// Created by glom on 9/25/25.
//

#ifndef GLOM_TOKENIZER_H
#define GLOM_TOKENIZER_H
#include <memory>
#include <string>
#include <variant>

using std::string;
using std::variant;
using std::monostate;

enum TokenType
{
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_BOOLEAN,
    TOKEN_SYMBOL,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_QUOTE,
    TOKEN_EOI
};


class Token
{
    TokenType type;
    variant<double, string, bool, monostate> value;

    explicit Token(double x);
    explicit Token(bool x);
    explicit Token(TokenType token);
    Token(TokenType token, string x);
public:
    TokenType getType() const;

     double asNumber() const;
    bool asBoolean() const;
    string& asString();

    static Token makeNumber(double x);
    static Token makeString(string x);
    static Token makeBoolean(bool x);
    static Token makeSymbol(string x);
    static Token makeLParen();
    static Token makeRParen();
    static Token makeQuote();
    static Token makeEOI();
};


class Tokenizer
{
    string input;
    size_t index;

    Token nextNumber();
    Token nextString();
    Token nextSymbolOrBoolean();

public:
    explicit Tokenizer(string input);

    Token next();

};

#endif //GLOM_TOKENIZER_H