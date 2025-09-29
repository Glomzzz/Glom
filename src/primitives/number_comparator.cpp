//
// Created by glom on 9/27/25.
//
#include "error.h"
#include "expr.h"
#include "context.h"
#include "primitive.h"

shared_ptr<Expr> primitives::eq(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty() || args->cdr()->is_nil())
    {
        throw GlomError("Invalid number of arguments =: at least two arguments required");
    }
    const auto first_expr = context->eval(std::move(args->car()));
    if (first_expr->get_type() != NUMBER)
    {
        throw GlomError("Invalid argument =: " + first_expr->to_string() + " is not a number");
    }
    const auto rest = args->cdr()->as_pair();
    const auto first = first_expr->as_number();
    for (const auto it: *rest)
    {
        if (!it) break;
        const auto next_expr = context->eval(it);
        if (next_expr->get_type() != NUMBER)
        {
            throw GlomError("Invalid argument =: " + next_expr->to_string() + " is not a number");
        }
        if (next_expr->as_number() != first)
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
    const auto first = context->eval(std::move(args->car()));
    if (first->get_type() != NUMBER)
    {
        throw GlomError("Invalid argument <: " + first->to_string() + " is not a number");
    }
    const auto rest = args->cdr()->as_pair();
    double last = first->as_number();
    for (const auto it: *rest)
    {
        if (!it) break;
        const auto next_expr = context->eval(it);
        if (next_expr->get_type() != NUMBER)
        {
            throw GlomError("Invalid argument <: " + next_expr->to_string() + " is not a number");
        }
        const double next = next_expr->as_number();
        if (last >= next)
        {
            return Expr::FALSE;
        }
        last = next;
    }
    return Expr::TRUE;
}
shared_ptr<Expr> primitives::gt(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty() || args->cdr()->is_nil())
    {
        throw GlomError("Invalid number of arguments >: at least two arguments required");
    }
    const auto first = context->eval(std::move(args->car()));
    if (first->get_type() != NUMBER)
    {
        throw GlomError("Invalid argument >: " + first->to_string() + " is not a number");
    }
    const auto rest = args->cdr()->as_pair();
    double last = first->as_number();
    for (const auto it: *rest)
    {
        if (!it) break;
        const auto next_expr = context->eval(it);

        if (next_expr->get_type() != NUMBER)
        {
            throw GlomError("Invalid argument >: " + next_expr->to_string() + " is not a number");
        }
        const double next = next_expr->as_number();
        if (last <= next)
        {
            return Expr::FALSE;
        }
        last = next;
    }
    return Expr::TRUE;
}

shared_ptr<Expr> primitives::le(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty() || args->cdr()->is_nil())
    {
        throw GlomError("Invalid number of arguments <=: at least two arguments required");
    }
    const auto first = context->eval(std::move(args->car()));
    if (first->get_type() != NUMBER)
    {
        throw GlomError("Invalid argument <=: " + first->to_string() + " is not a number");
    }
    const auto rest = args->cdr()->as_pair();
    double last = first->as_number();
    for (const auto it: *rest)
    {
        if (!it) break;
        const auto next_expr = context->eval(it);

        if (next_expr->get_type() != NUMBER)
        {
            throw GlomError("Invalid argument <=: " + next_expr->to_string() + " is not a number");
        }
        const double next = next_expr->as_number();
        if (last > next)
        {
            return Expr::FALSE;
        }
        last = next;
    }
    return Expr::TRUE;
}
shared_ptr<Expr> primitives::ge(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty() || args->cdr()->is_nil())
    {
        throw GlomError("Invalid number of arguments >=: at least two arguments required");
    }
    const auto first = context->eval(std::move(args->car()));
    if (first->get_type() != NUMBER)
    {
        throw GlomError("Invalid argument >=: " + first->to_string() + " is not a number");
    }
    const auto rest = args->cdr()->as_pair();
    double last = first->as_number();
    for (const auto it: *rest)
    {
        if (!it) break;
        const auto next_expr = context->eval(it);

        if (next_expr->get_type() != NUMBER)
        {
            throw GlomError("Invalid argument >=: " + next_expr->to_string() + " is not a number");
        }
        const double next = next_expr->as_number();
        if (last < next)
        {
            return Expr::FALSE;
        }
        last = next;
    }
    return Expr::TRUE;
}