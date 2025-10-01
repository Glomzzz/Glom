//
// Created by glom on 9/27/25.
//
#include <cmath>

#include "context.h"
#include "error.h"
#include "expr.h"
#include "primitive.h"

shared_ptr<Expr> generic_add(shared_ptr<Expr>& a, shared_ptr<Expr>& b)
{
    primitives_utils::coerce_number(a, b);
    if (a->is_number_real())
    {
        return Expr::make_number_real(a->as_number_real() + b->as_number_real());
    }
    if (a->is_number_rat())
    {
        auto result = a->as_number_rat() + b->as_number_rat();
        return Expr::make_number_exact(std::move(result));
    }
    return Expr::make_number_int(a->as_number_int() + b->as_number_int());
}

shared_ptr<Expr> generic_sub(shared_ptr<Expr>& a, shared_ptr<Expr>& b)
{
    primitives_utils::coerce_number(a, b);
    if (a->is_number_real())
    {
        return Expr::make_number_real(a->as_number_real() - b->as_number_real());
    }
    if (a->is_number_rat())
    {
        auto result = a->as_number_rat() - b->as_number_rat();
        return Expr::make_number_exact(std::move(result));
    }
    return Expr::make_number_int(a->as_number_int() - b->as_number_int());
}

shared_ptr<Expr> generic_mul(shared_ptr<Expr>& a, shared_ptr<Expr>& b)
{
    primitives_utils::coerce_number(a, b);
    if (a->is_number_real())
    {
        return Expr::make_number_real(a->as_number_real() * b->as_number_real());
    }
    if (a->is_number_rat())
    {
        auto result = a->as_number_rat() * b->as_number_rat();
        return Expr::make_number_exact(std::move(result));
    }
    return Expr::make_number_int(a->as_number_int() * b->as_number_int());
}

shared_ptr<Expr> generic_div(shared_ptr<Expr>& a, shared_ptr<Expr>& b)
{
    primitives_utils::coerce_number(a, b);
    if (a->is_number_real())
    {
        return Expr::make_number_real(a->as_number_real() / b->as_number_real());
    }
    rational result;
    if (a->is_number_rat())
    {
        result = a->as_number_rat() / b->as_number_rat();
    }
    else
    {
        result = rational(a->as_number_int(), b->as_number_int());
    }
    return Expr::make_number_exact(std::move(result));
}

shared_ptr<Expr> generic_neg(const shared_ptr<Expr>& a)
{
    if (a->is_number_real())
    {
        return Expr::make_number_real(-a->as_number_real());
    }
    if (a->is_number_rat())
    {
        auto result = -a->as_number_rat();
        return Expr::make_number_exact(std::move(result));
    }
    return Expr::make_number_int(-a->as_number_int());
}

shared_ptr<Expr> generic_reciprocal(const shared_ptr<Expr>& a)
{
    if (a->is_number_real())
    {
        if (a->as_number_real() == 0.0)
        {
            throw GlomError("Division by zero");
        }
        return Expr::make_number_real(1.0 / a->as_number_real());
    }
    if (a->is_number_rat())
    {
        auto rat = a->as_number_rat();
        if (rat.num.is_zero())
        {
            throw GlomError("Division by zero");
        }
        auto result = rational(std::move(rat.den), std::move(rat.num));
        return Expr::make_number_exact(std::move(result));
    }
    if (a->as_number_int().is_zero())
    {
        throw GlomError("Division by zero");
    }
    auto result = rational(integer(1), std::move(a->as_number_int()));
    return Expr::make_number_exact(std::move(result));
}


shared_ptr<Expr> pow(const shared_ptr<Expr>& a, const shared_ptr<Expr>& b)
{
    if (a->is_number_real() || b->is_number_real())
    {
        return Expr::make_number_real(std::powl(a->to_number_real(), b->to_number_real()));
    }
    integer exp;
    if (b->is_number_rat())
    {
        const auto rat_exp = b->as_number_rat();
        if (!rat_exp.is_integer())
        {
            return Expr::make_number_real(std::powl(a->to_number_real(), rat_exp.to_inexact()));
        }
        exp = rat_exp.num;
    } else
    {
        exp = b->as_number_int();
    }
    rational result;
    if (a->is_number_rat())
    {
        result = a->as_number_rat().pow(exp);
    }
    else
    {
        result = rational(a->as_number_int()).pow(exp);
    }
    return Expr::make_number_exact(std::move(result));
}

shared_ptr<Expr> primitives::add(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty())
    {
        return Expr::make_number_int(integer(0));
    }

    shared_ptr<Expr> sum = nullptr;

    for (auto expr : *args)
    {
        if (!expr) break;
        auto arg = context->eval(std::move(expr));
        if (!arg->is_number())
        {
            throw GlomError("Invalid argument +: " + arg->to_string());
        }
        if (!sum)
        {
            sum = arg;
            continue;
        }
        sum = generic_add(sum, arg);
    }
    return sum;
}
shared_ptr<Expr> primitives::sub(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty())
    {
        throw GlomError("Invalid number of arguments -: at least one argument required");
    }
    const auto minuend_expr = context->eval(args->car());
    if (!minuend_expr->is_number())
    {
        throw GlomError("Invalid argument -: " + minuend_expr->to_string());
    }
    if (args->cdr()->is_nil())
    {
        return generic_neg(minuend_expr);
    }
    shared_ptr<Expr> minuend = minuend_expr;
    for (const auto subtrahends = args->cdr()->as_pair(); auto expr : *subtrahends)
    {
        if (!expr) break;
        auto arg = context->eval(std::move(expr));
        if (!arg->is_number())
        {
            throw GlomError("Invalid argument -: " + arg->to_string());
        }
        minuend = generic_sub(minuend, arg);
    }
    return minuend;
}
shared_ptr<Expr> primitives::mul(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty())
    {
        return Expr::make_number_int(integer(1));
    }

    shared_ptr<Expr> product = nullptr;
    for (auto expr : *args)
    {
        if (!expr) break;
        auto arg = context->eval(std::move(expr));
        if (!arg->is_number())
        {
            throw GlomError("Invalid argument *: " + arg->to_string());
        }
        if (!product)
        {
            product = arg;
            continue;
        }
        product = generic_mul(product, arg);
    }
    return product;
}
shared_ptr<Expr> primitives::div(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty())
    {
        throw GlomError("Invalid number of arguments /: at least one argument required");
    }
    const auto dividend_expr = context->eval(args->car());
    if (!dividend_expr->is_number())
    {
        throw GlomError("Invalid argument /: " + dividend_expr->to_string());
    }
    if (args->cdr()->is_nil())
    {
        return generic_reciprocal(dividend_expr);
    }
    shared_ptr<Expr> dividend = dividend_expr;
    for (const auto divisors = args->cdr()->as_pair(); auto expr : *divisors)
    {
        if (!expr) break;
        auto arg = context->eval(std::move(expr));
        if (!arg->is_number())
        {
            throw GlomError("Invalid argument /: " + arg->to_string());
        }
        dividend = generic_div(dividend, arg);
    }

    return dividend;
}

shared_ptr<Expr> primitives::remainder(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> a,b = nullptr;
    primitives_utils::expect_2_args("remainder", args, a, b);
    a = context->eval(std::move(a));
    b = context->eval(std::move(b));

    if (!a->is_number_int() || !b->is_number_int())
    {
        throw GlomError("remainder only support integer numbers");
    }
    const integer dividend = a->as_number_int();
    const integer divisor = b->as_number_int();
    if (divisor.is_zero())
    {
        throw GlomError("remainder undefined for 0");
    }
    integer result = dividend.remainder(divisor);
    return Expr::make_number_int(std::move(result));
}

shared_ptr<Expr> primitives::modulo(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> a,b = nullptr;
    primitives_utils::expect_2_args("modulo", args, a, b);
    a = context->eval(std::move(a));
    b = context->eval(std::move(b));

    if (!a->is_number_int() || !b->is_number_int())
    {
        throw GlomError("modulo only support integer numbers");
    }
    const integer dividend = a->as_number_int();
    const integer divisor = b->as_number_int();
    if (divisor.is_zero())
    {
        throw GlomError("modulo undefined for 0");
    }
    integer result = dividend.modulo(divisor);
    return Expr::make_number_int(std::move(result));
}


shared_ptr<Expr> primitives::power(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> base_expr, exp_expr = nullptr;
    primitives_utils::expect_2_args("expt", args, base_expr, exp_expr);
    base_expr = context->eval(std::move(base_expr));
    exp_expr = context->eval(std::move(exp_expr));
    if (!base_expr->is_number() || !exp_expr->is_number())
    {
        throw GlomError("Invalid argument expt: both arguments must be numbers");
    }
    return pow(base_expr, exp_expr);
}