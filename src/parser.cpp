//
// Created by glom on 9/25/25.
//

#include "parser.h"
#include "tokenizer.h"

class Parser
{
    Tokenizer tokenizer;

public:
    explicit Parser(const string& input) : tokenizer(input)
    {
    }

    Expr parse_with(Token& token)
    {
        switch (token.type)
        {
        case TOKEN_NUMBER:
            return Expr(std::get<double>(token.value));
        case TOKEN_BOOLEAN:
            return Expr(std::get<bool>(token.value));
        case TOKEN_STRING:
            return {STRING, std::get<string>(token.value)};
        case TOKEN_SYMBOL:
            return {SYMBOL, std::get<string>(token.value)};
        case TOKEN_LPAREN:
            {
                vector<unique_ptr<Expr>> elements;
                while (true)
                {
                    Token nextToken = tokenizer.next();
                    if (nextToken.type == TOKEN_RPAREN)
                    {
                        break;
                    }
                    elements.push_back(std::make_unique<Expr>(parse_with(nextToken)));
                }
                return Expr(elements);
            }
        case TOKEN_QUOTE:
            {
                vector<unique_ptr<Expr>> elements;
                elements.push_back(std::make_unique<Expr>(Expr{SYMBOL, "quote"}));
                Token nextToken = tokenizer.next();
                elements.push_back(std::make_unique<Expr>(parse_with(nextToken)));
                return Expr(elements);
            }
        case TOKEN_RPAREN:
            throw std::runtime_error("Unexpected ')'");
        case TOKEN_EOI:
            throw std::runtime_error("Unexpected end of input");
        default:
            throw std::runtime_error("Unknown token type of " + std::to_string(token.type));
        }
    }

    vector<unique_ptr<Expr>> parse()
    {
        vector<unique_ptr<Expr>> result;
        Token token = tokenizer.next();
        while (token.type != TOKEN_EOI)
        {
            result.push_back(std::make_unique<Expr>(parse_with(token)));
            token = tokenizer.next();
        }
        return result;
    }
};


vector<unique_ptr<Expr>> parse(const string& input)
{
    Parser parser(input);
    return parser.parse();
}
