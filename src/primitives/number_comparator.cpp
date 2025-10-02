//
// Created by glom on 9/27/25.
//
#include "error.h"
#include "expr.h"
#include "context.h"
#include "primitive.h"

shared_ptr<Expr> primitives_utils::continuation(const shared_ptr<Context>& context, shared_ptr<Pair>&& exprs)
{
    return Expr::make_cont(std::make_unique<Continuation>(context, std::move(exprs)));
}

bool primitives_utils::generic_num_eq(shared_ptr<Expr>& a, shared_ptr<Expr>& b)
{
    coerce_number(a,b);
    if (a->is_number_real())
    {
        return a->as_number_real() == b->as_number_real();
    }
    if (a->is_number_rat())
    {
        return a->as_number_rat() == b->as_number_rat();
    }
    return a->as_number_int() == b->as_number_int();
}

bool generic_lt(shared_ptr<Expr>& a, shared_ptr<Expr>& b)
{
    primitives_utils::coerce_number(a,b);
    if (a->is_number_real())
    {
        return a->as_number_real() < b->as_number_real();
    }
    if (a->is_number_rat())
    {
        return a->as_number_rat() < b->as_number_rat();
    }
    return a->as_number_int() < b->as_number_int();
}

bool generic_gt(shared_ptr<Expr>& a, shared_ptr<Expr>& b)
{
    primitives_utils::coerce_number(a,b);
    if (a->is_number_real())
    {
        return a->as_number_real() > b->as_number_real();
    }
    if (a->is_number_rat())
    {
        return a->as_number_rat() > b->as_number_rat();
    }
    return a->as_number_int() > b->as_number_int();
}

bool generic_le(shared_ptr<Expr>& a, shared_ptr<Expr>& b)
{
    primitives_utils::coerce_number(a,b);
    if (a->is_number_real())
    {
        return a->as_number_real() <= b->as_number_real();
    }
    if (a->is_number_rat())
    {
        return a->as_number_rat() <= b->as_number_rat();
    }
    return a->as_number_int() <= b->as_number_int();
}

bool generic_ge(shared_ptr<Expr>& a, shared_ptr<Expr>& b)
{
    primitives_utils::coerce_number(a,b);
    if (a->is_number_real())
    {
        return a->as_number_real() >= b->as_number_real();
    }
    if (a->is_number_rat())
    {
        return a->as_number_rat() >= b->as_number_rat();
    }
    return a->as_number_int() >= b->as_number_int();
}

shared_ptr<Expr> primitives::eq(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty() || args->cdr()->is_nil())
    {
        throw GlomError("Invalid number of arguments =: at least two arguments required");
    }
    const auto first_expr = eval(context, std::move(args->car()));
    if (!first_expr->is_number())
    {
        throw GlomError("Invalid argument =: " + first_expr->to_string() + " is not a number");
    }
    const auto rest = args->cdr()->as_pair();
    auto first = first_expr;
    for (const auto it: *rest)
    {
        if (!it) break;
        auto next_expr = eval(context, it);
        if (!next_expr->is_number())
        {
            throw GlomError("Invalid argument =: " + next_expr->to_string() + " is not a number");
        }
        if (!primitives_utils::generic_num_eq(first, next_expr))
        {
            return Expr::FALSE;
        }
    }
    return Expr::TRUE;
}
shared_ptr<Expr> primitives::lt(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty() || args->cdr()->is_nil())
    {
        throw GlomError("Invalid number of arguments <: at least two arguments required");
    }
    const auto first = eval(context, std::move(args->car()));
    if (!first->is_number())
    {
        throw GlomError("Invalid argument <: " + first->to_string() + " is not a number");
    }
    const auto rest = args->cdr()->as_pair();
    auto last = first;
    for (const auto it: *rest)
    {
        if (!it) break;
        auto next_expr = eval(context, it);
        if (!next_expr->is_number())
        {
            throw GlomError("Invalid argument <: " + next_expr->to_string() + " is not a number");
        }
        if (generic_ge(last, next_expr))
        {
            return Expr::FALSE;
        }
        last = next_expr;
    }
    return Expr::TRUE;
}
shared_ptr<Expr> primitives::gt(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty() || args->cdr()->is_nil())
    {
        throw GlomError("Invalid number of arguments >: at least two arguments required");
    }
    const auto first = eval(context, std::move(args->car()));
    if (!first->is_number())
    {
        throw GlomError("Invalid argument >: " + first->to_string() + " is not a number");
    }
    const auto rest = args->cdr()->as_pair();
    auto last = first;
    for (const auto it: *rest)
    {
        if (!it) break;
         auto next_expr = eval(context, it);

        if (!next_expr->is_number())
        {
            throw GlomError("Invalid argument >: " + next_expr->to_string() + " is not a number");
        }
        if (generic_le(last, next_expr))
        {
            return Expr::FALSE;
        }
        last = next_expr;
    }
    return Expr::TRUE;
}

shared_ptr<Expr> primitives::le(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty() || args->cdr()->is_nil())
    {
        throw GlomError("Invalid number of arguments <=: at least two arguments required");
    }
    const auto first = eval(context, std::move(args->car()));
    if (!first->is_number())
    {
        throw GlomError("Invalid argument <=: " + first->to_string() + " is not a number");
    }
    const auto rest = args->cdr()->as_pair();
    auto last = first;
    for (const auto it: *rest)
    {
        if (!it) break;
        auto next_expr = eval(context, it);

        if (!next_expr->is_number())
        {
            throw GlomError("Invalid argument <=: " + next_expr->to_string() + " is not a number");
        }
        if (generic_gt(last, next_expr))
        {
            return Expr::FALSE;
        }
        last = next_expr;
    }
    return Expr::TRUE;
}
shared_ptr<Expr> primitives::ge(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty() || args->cdr()->is_nil())
    {
        throw GlomError("Invalid number of arguments >=: at least two arguments required");
    }
    const auto first = eval(context, std::move(args->car()));
    if (!first->is_number())
    {
        throw GlomError("Invalid argument >=: " + first->to_string() + " is not a number");
    }
    const auto rest = args->cdr()->as_pair();
    auto last = first;
    for (const auto it: *rest)
    {
        if (!it) break;
        auto next_expr = eval(context, it);

        if (!next_expr->is_number())
        {
            throw GlomError("Invalid argument >=: " + next_expr->to_string() + " is not a number");
        }
        if (generic_lt(last, next_expr))
        {
            return Expr::FALSE;
        }
        last = next_expr;
    }
    return Expr::TRUE;
}