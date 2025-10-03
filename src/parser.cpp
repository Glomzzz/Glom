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
        case TOKEN_NUMBER_INT:
            return Expr::make_number_int(token.as_number_int());
        case TOKEN_NUMBER_RAT:
        {
            auto rat = token.as_number_rat();
            return Expr::make_number_exact(std::move(rat));
        }
        case TOKEN_NUMBER_REAL:
            return Expr::make_number_real(token.as_number_real());
        case TOKEN_BOOLEAN:
            {
                if (token.as_boolean())
                {
                    return Expr::TRUE;
                }
                return Expr::FALSE;
            }
        case TOKEN_STRING:
            return Expr::make_string(std::make_unique<string>(token.as_string()));
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
                    return nullptr;
                return Expr::make_pair(pair);
            }
        case TOKEN_QUOTE:
            {
                const auto pair = Pair::single(Expr::make_symbol("quote"));
                auto data = parse_with(std::move(tokenizer.next()));
                if (!data)
                    data = Expr::NIL;
                pair->set_cdr(Expr::make_pair(Pair::single(data)));
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

    shared_ptr<Pair> parse()
    {
        shared_ptr<Pair> result = nullptr;
        shared_ptr<Pair> current = nullptr;
        Token token = tokenizer.next();
        while (token.get_type() != TOKEN_EOI)
        {
            auto next = parse_with(std::move(token));
            if (!next)
                throw std::runtime_error("invalid syntax ()");
            if (!result)
            {
                result = Pair::single(std::move(next));
                current = result;
            }
            else
            {
                const auto next_pair = Pair::single(std::move(next));
                current->set_cdr(Expr::make_pair(next_pair));
                current = next_pair;
            }
            token = tokenizer.next();
        }
        return result;
    }
};

shared_ptr<Expr> parse_expr(string input)
{
    const auto exprs = parse(std::move(input));
    if (exprs->empty())
    {
        throw std::runtime_error("Expected a single expression, got " + exprs->to_string());
    }
    return exprs->car();
}
shared_ptr<Pair> parse(string input)
{
    Parser parser(std::move(input));
    return parser.parse();
}
