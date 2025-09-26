//
// Created by glom on 9/25/25.
//

#include "parser.h"
#include "tokenizer.h"

class Parser
{
    Tokenizer tokenizer;

public:
    explicit Parser(string input) : tokenizer(std::move(input)){}

    Expr parse_with(Token& token)
    {
        switch (token.get_type())
        {
        case TOKEN_NUMBER:
            return Expr::make_number(token.as_number());
        case TOKEN_BOOLEAN:
            return Expr::make_boolean(token.as_boolean());
        case TOKEN_STRING:
            return Expr::make_string(std::move(token.as_string()));
        case TOKEN_SYMBOL:
            return Expr::make_symbol(std::move(token.as_string()));
        case TOKEN_LPAREN:
            {
                vector<shared_ptr<Expr>> elements;
                while (true)
                {
                    Token nextToken = tokenizer.next();
                    if (nextToken.get_type() == TOKEN_RPAREN)
                    {
                        break;
                    }
                    elements.push_back(std::make_shared<Expr>(parse_with(nextToken)));
                }
                return Expr::make_list(elements);
            }
        case TOKEN_QUOTE:
            {
                vector<shared_ptr<Expr>> elements;
                elements.push_back(std::make_shared<Expr>(Expr::make_symbol("quote")));
                Token nextToken = tokenizer.next();
                elements.push_back(std::make_shared<Expr>(parse_with(nextToken)));
                return Expr::make_list(elements);
            }
        case TOKEN_RPAREN:
            throw std::runtime_error("Unexpected ')'");
        case TOKEN_EOI:
            throw std::runtime_error("Unexpected end of input");
        default:
            throw std::runtime_error("Unknown token type of " + std::to_string(token.get_type()));
        }
    }

    vector<shared_ptr<Expr>> parse()
    {
        vector<shared_ptr<Expr>> result;
        Token token = tokenizer.next();
        while (token.get_type() != TOKEN_EOI)
        {
            result.push_back(std::make_shared<Expr>(parse_with(token)));
            token = tokenizer.next();
        }
        return result;
    }
};
vector<shared_ptr<Expr>> parse(string&& input)
{
    Parser parser(std::move(input));
    return parser.parse();
}
vector<shared_ptr<Expr>> parse(const string& input)
{
    Parser parser(input);
    return parser.parse();
}

