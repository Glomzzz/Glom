//
// Created by glom on 9/25/25.
//

#include "parser.h"
#include "tokenizer.h"
#include "expr.h"

class Parser
{
    Tokenizer tokenizer;

public:
    explicit Parser(string input) : tokenizer(std::move(input)){}

    shared_ptr<Expr> parse_with(Token&& token)
    {
        switch (token.get_type())
        {
        case TOKEN_NUMBER:
            return Expr::make_number(token.as_number());
        case TOKEN_BOOLEAN:
            {
                if (token.as_boolean())
                {
                    return Expr::TRUE;
                }
                return Expr::FALSE;
            }
        case TOKEN_STRING:
            return Expr::make_string(std::move(token.as_string()));
        case TOKEN_SYMBOL:
            return Expr::make_symbol(std::move(token.as_string()));
        case TOKEN_LPAREN:
            {
                shared_ptr<Pair> pair = nullptr;
                shared_ptr<Pair> current = nullptr;
                while (true)
                {
                    Token nextToken = tokenizer.next();
                    if (nextToken.get_type() == TOKEN_RPAREN)
                    {
                        break;
                    }
                    auto next_pair = Pair::single(parse_with(std::move(nextToken)));
                    if (!pair)
                    {
                        pair = std::move(next_pair);
                        current = pair;
                        continue;
                    }
                    current->set_cdr(Expr::make_pair(next_pair));
                    current = next_pair;
                }
                if (!pair)
                    return Expr::NIL;
                return Expr::make_pair(pair);
            }
        case TOKEN_QUOTE:
            {
                const auto pair = Pair::single(Expr::make_symbol("quote"));
                pair->set_cdr(parse_with(std::move(tokenizer.next())));
                return Expr::make_pair(pair);
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
            result.push_back(parse_with(std::move(token)));
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

