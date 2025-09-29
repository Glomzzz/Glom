//
// Created by glom on 9/27/25.
//

#include "error.h"
#include "context.h"
#include "expr.h"
#include "primitive.h"

shared_ptr<Expr> primitives::is_null(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> expr = nullptr;
    primitives_utils::expect_1_arg("null?", args, expr);
    expr = context->eval(expr);
    return Expr::make_boolean(expr->is_nil());
}
shared_ptr<Expr> primitives::cons(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> a, b = nullptr;
    primitives_utils::expect_2_args("cons", args, a, b);
    a = context->eval(a);
    b = context->eval(b);
    return Expr::make_pair(Pair::cons(a,b));
}
shared_ptr<Expr> primitives::car(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> pair_expr = nullptr;
    primitives_utils::expect_1_arg("car", args, pair_expr);
    pair_expr = context->eval(pair_expr);
    if (pair_expr->get_type() != PAIR)
    {
        throw GlomError("car: argument is not a pair");
    }
    const auto pair = pair_expr->as_pair();
    if (pair->empty())
    {
        throw GlomError("car: cannot take car of empty list");
    }
    return pair->car();
}
shared_ptr<Expr> primitives::cdr(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> pair_expr = nullptr;
    primitives_utils::expect_1_arg("car", args, pair_expr);
    pair_expr = context->eval(pair_expr);
    if (pair_expr->get_type() != PAIR)
    {
        throw GlomError("cdr: argument is not a pair");
    }
    const auto pair = pair_expr->as_pair();
    if (pair->empty())
    {
        throw GlomError("cdr: cannot take car of empty list");
    }
    return pair->cdr();
}
shared_ptr<Expr> primitives::list(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Pair> list = nullptr;
    shared_ptr<Pair> tail = nullptr;
    for (auto elem_expr : *args)
    {
        if (!elem_expr) break;
        auto last = context->eval(std::move(elem_expr));
        if (!list)
        {
            list = Pair::single(std::move(last));
            tail = list;
        }
        else
        {
            const auto new_tail = Pair::single(std::move(last));
            tail->set_cdr(Expr::make_pair(new_tail));
            tail = new_tail;
        }
    }
    if (!list)
        return Expr::NIL;
    return Expr::make_pair(list);
}