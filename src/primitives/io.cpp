//
// Created by glom on 9/27/25.
//

#include <iostream>
#include <parser.h>

#include "error.h"
#include "context.h"
#include "expr.h"
#include "primitive.h"

shared_ptr<Expr> primitives::display(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty())
    {
        throw GlomError("incorrect argument count in call display");
    }
    for (auto expr : *args)
    {
        if (!expr) break;
        const auto arg = context->eval(std::move(expr));
        std::cout << arg->to_string();
    }
    return Expr::NOTHING;
}

shared_ptr<Expr> primitives::read(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (!args->empty())
    {
        throw GlomError("incorrect argument count in call read: expected 0 arguments");
    }
    std::string line;
    if (!std::getline(std::cin, line))
    {
        throw GlomError("Failed to read input");
    }
    return parse_expr(std::move(line));
}
shared_ptr<Expr> primitives::newline(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (!args->empty())
    {
        throw GlomError("incorrect argument count in call newline: expected 0 arguments");
    }
    std::cout << std::endl;
    return Expr::NOTHING;
}