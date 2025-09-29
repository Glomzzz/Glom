//
// Created by glom on 9/27/25.
//

#include "error.h"
#include "context.h"
#include "expr.h"
#include "primitive.h"

shared_ptr<Expr> primitives::logical_and(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    for (auto expr : *args)
    {
        if (!expr) break;
        if (!context->eval(std::move(expr))->to_boolean())
        {
            return Expr::FALSE;
        }
    }
    return Expr::TRUE;
}
shared_ptr<Expr> primitives::logical_or(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    for (auto expr : *args)
    {
        if (!expr) break;
        if (context->eval(std::move(expr))->to_boolean())
        {
            return Expr::TRUE;
        }
    }
    return Expr::FALSE;
}
shared_ptr<Expr> primitives::logical_not(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> expr = nullptr;
    primitives_utils::expect_1_arg("not", args, expr);
    expr = context->eval(expr);
    return Expr::make_boolean(!expr->to_boolean());
}