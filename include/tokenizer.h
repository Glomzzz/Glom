//
// Created by glom on 9/25/25.
//

#ifndef GLOM_TOKENIZER_H
#define GLOM_TOKENIZER_H
#include <memory>
#include <string>
#include <variant>
#include "type.h"

using std::string;
using std::variant;
using std::monostate;

enum TokenType
{
    TOKEN_NUMBER_INT,
    TOKEN_NUMBER_RAT,
    TOKEN_NUMBER_REAL,
    TOKEN_STRING,
    TOKEN_SYMBOL,
    TOKEN_BOOLEAN,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_QUOTE,
    TOKEN_EOI
};


class Token
{
    TokenType type;
    variant<monostate, bool, integer, rational, real, string> value;

    explicit Token(integer x);
    explicit Token(rational x);
    explicit Token(real x);
    explicit Token(bool x);
    explicit Token(TokenType token);
    Token(TokenType token, string x);
public:
    [[nodiscard]] TokenType get_type() const;
    [[nodiscard]] integer as_number_int() const;
    [[nodiscard]] rational as_number_rat() const;
    [[nodiscard]] real as_number_real() const;
    [[nodiscard]] bool as_boolean() const;
    string& as_string();

    static Token make_number_int(integer x);
    static Token make_number_rat(rational x);
    static Token make_number_real(real x);
    static Token make_string(string x);
    static Token make_boolean(bool x);
    static Token make_symbol(string x);
    static Token make_left_paren();
    static Token make_right_paren();
    static Token make_quote();
    static Token make_end_of_input();
};


class Tokenizer
{
    string input;
    size_t index;

    Token next_number();
    Token next_string();
    Token next_symbol_or_boolean();

public:
    explicit Tokenizer(string input);

    Token next();

};

#endif //GLOM_TOKENIZER_H