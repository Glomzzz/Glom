//
// Created by glom on 9/27/25.
//
#include <cmath>

#include "context.h"
#include "error.h"
#include "expr.h"
#include "primitive.h"

shared_ptr<Expr> primitives::add(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty())
    {
        return Expr::make_number(0.0);
    }

    double sum = 0;

    for (auto expr : *args)
    {
        if (!expr) break;
        const auto arg = context->eval(std::move(expr));
        if (arg->get_type() != NUMBER)
        {
            throw GlomError("Invalid argument +: " + arg->to_string());
        }
        sum += arg->as_number();
    }

    return Expr::make_number(sum);
}
shared_ptr<Expr> primitives::sub(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty())
    {
        throw GlomError("Invalid number of arguments -: at least one argument required");
    }
    const auto minuend_expr = context->eval(args->car());
    if (minuend_expr->get_type() != NUMBER)
    {
        throw GlomError("Invalid argument -: " + minuend_expr->to_string());
    }
    if (args->cdr()->is_nil())
    {
        return Expr::make_number(-minuend_expr->as_number());
    }
    double minuend = minuend_expr->as_number();
    for (const auto subtrahends = args->cdr()->as_pair(); auto expr : *subtrahends)
    {
        if (!expr) break;
        const auto arg = context->eval(std::move(expr));
        if (arg->get_type() != NUMBER)
        {
            throw GlomError("Invalid argument -: " + arg->to_string());
        }
        minuend -= arg->as_number();
    }

    return Expr::make_number(minuend);
}
shared_ptr<Expr> primitives::mul(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty())
    {
        return Expr::make_number(1.0);
    }

    double product = 1;
    for (auto expr : *args)
    {
        if (!expr) break;
        const auto arg = context->eval(std::move(expr));
        if (arg->get_type() != NUMBER)
        {
            throw GlomError("Invalid argument *: " + arg->to_string());
        }
        product *= arg->as_number();
    }
    return Expr::make_number(product);
}
shared_ptr<Expr> primitives::div(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty())
    {
        throw GlomError("Invalid number of arguments /: at least one argument required");
    }
    const auto dividend_expr = context->eval(args->car());
    if (dividend_expr->get_type() != NUMBER)
    {
        throw GlomError("Invalid argument /: " + dividend_expr->to_string());
    }
    if (args->cdr()->is_nil())
    {
        return Expr::make_number(1 / dividend_expr->as_number());
    }
    double dividend = dividend_expr->as_number();
    for (const auto divisors = args->cdr()->as_pair(); auto expr : *divisors)
    {
        if (!expr) break;
        const auto arg = context->eval(std::move(expr));
        if (arg->get_type() != NUMBER)
        {
            throw GlomError("Invalid argument -: " + arg->to_string());
        }
        const auto divisor = arg->as_number();
        if (divisor == 0.0)
        {
            throw GlomError("Division by zero /");
        }
        dividend /= divisor;
    }

    return Expr::make_number(dividend);
}
shared_ptr<Expr> primitives::remainder(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> a,b = nullptr;
    primitives_utils::expect_2_args("remainder", args, a, b);
    a = context->eval(std::move(a));
    b = context->eval(std::move(b));
    if (a->get_type() != NUMBER || b->get_type() != NUMBER)
    {
        throw GlomError("Invalid argument remainder: both arguments must be numbers");
    }
    const double divisor = b->as_number();
    if (divisor == 0.0)
    {
        throw GlomError("Division by zero remainder");
    }
    const int dividend = static_cast<int>(a->as_number());
    const double result = std::fmod(dividend, divisor);
    return Expr::make_number(result);
}

shared_ptr<Expr> primitives::power(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> base_expr, exp_expr = nullptr;
    primitives_utils::expect_2_args("expt", args, base_expr, exp_expr);
    base_expr = context->eval(std::move(base_expr));
    exp_expr = context->eval(std::move(exp_expr));
    if (base_expr->get_type() != NUMBER || exp_expr->get_type() != NUMBER)
    {
        throw GlomError("Invalid argument expt: both arguments must be numbers");
    }
    const double base = base_expr->as_number();
    const double exp = exp_expr->as_number();
    const double result = std::pow(base, exp);
    return Expr::make_number(result);
}