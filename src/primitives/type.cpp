//
// Created by glom on 9/27/25.
//

#include "error.h"
#include "context.h"
#include "expr.h"
#include "primitive.h"

shared_ptr<Expr> primitives::is_pair(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> expr = nullptr;
    primitives_utils::expect_1_arg("pair?", args, expr);
    expr = context->eval(std::move(expr));
    return Expr::make_boolean(expr->is_pair());
}
shared_ptr<Expr> primitives::is_number(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> expr = nullptr;
    primitives_utils::expect_1_arg("number?", args, expr);
    expr = context->eval(std::move(expr));
    return Expr::make_boolean(expr->is_number());
}

