//
// Created by glom on 9/27/25.
//
#include <cmath>

#include "error.h"
#include "expr.h"
#include "context.h"
#include "primitive.h"

//is_zero
shared_ptr<Expr> primitives::is_zero(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty() || !args->cdr()->is_nil())
    {
        throw GlomError("Invalid number of arguments zero?: exactly one argument required");
    }
    const auto expr = eval(context, std::move(args->car()));
    if (!expr->is_number())
    {
        throw GlomError("Invalid argument zero?: " + expr->to_string() + " is not a number");
    }
    if (expr->is_number_int())
    {
        return Expr::make_boolean(expr->as_number_int().is_zero());
    }
    if (expr->is_number_rat())
    {
        return Expr::make_boolean(expr->as_number_rat().is_zero());
    }
    return Expr::make_boolean(expr->as_number_real() == 0);
}

// positive?
shared_ptr<Expr> primitives::is_positive(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty() || !args->cdr()->is_nil())
    {
        throw GlomError("Invalid number of arguments positive?: exactly one argument required");
    }
    const auto expr = eval(context, std::move(args->car()));
    if (!expr->is_number())
    {
        throw GlomError("Invalid argument positive?: " + expr->to_string() + " is not a number");
    }
    if (expr->is_number_int())
    {
        return Expr::make_boolean(expr->as_number_int().is_positive());
    }
    if (expr->is_number_rat())
    {
        return Expr::make_boolean(expr->as_number_rat().is_positive());
    }
    return Expr::make_boolean(expr->as_number_real() > 0);
}

// negative?
shared_ptr<Expr> primitives::is_negative(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty() || !args->cdr()->is_nil())
    {
        throw GlomError("Invalid number of arguments negative?: exactly one argument required");
    }
    const auto expr = eval(context, std::move(args->car()));
    if (!expr->is_number())
    {
        throw GlomError("Invalid argument negative?: " + expr->to_string() + " is not a number");
    }
    if (expr->is_number_int())
    {
        return Expr::make_boolean(expr->as_number_int().is_negative());
    }
    if (expr->is_number_rat())
    {
        return Expr::make_boolean(expr->as_number_rat().is_negative());
    }
    return Expr::make_boolean(expr->as_number_real() < 0);
}

// even?
shared_ptr<Expr> primitives::is_even(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty() || !args->cdr()->is_nil())
    {
        throw GlomError("Invalid number of arguments even?: exactly one argument required");
    }
    const auto expr = eval(context, std::move(args->car()));
    if (!expr->is_number())
    {
        throw GlomError("Invalid argument even?: " + expr->to_string() + " is not a number");
    }
    if (expr->is_number_int())
    {
        return Expr::make_boolean(expr->as_number_int().remainder(integer(2)).is_zero());
    }
    if (expr->is_number_rat())
    {
        const auto rat = expr->as_number_rat();
        if (!rat.den.is_one())
        {
            return Expr::FALSE;
        }
        return Expr::make_boolean(rat.num.remainder(integer(2)).is_zero());
    }
    const auto real_val = expr->as_number_real();
    if (std::floor(real_val) != real_val)
    {
        return Expr::FALSE;
    }
    return Expr::make_boolean(static_cast<int64_t>(real_val) % 2 == 0);
}

// odd?
shared_ptr<Expr> primitives::is_odd(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty() || !args->cdr()->is_nil())
    {
        throw GlomError("Invalid number of arguments odd?: exactly one argument required");
    }
    const auto expr = eval(context, std::move(args->car()));
    if (!expr->is_number())
    {
        throw GlomError("Invalid argument odd?: " + expr->to_string() + " is not a number");
    }
    if (expr->is_number_int())
    {
        return Expr::make_boolean(!expr->as_number_int().remainder(integer(2)).is_zero());
    }
    if (expr->is_number_rat())
    {
        const auto rat = expr->as_number_rat();
        if (!rat.den.is_one())
        {
            return Expr::FALSE;
        }
        return Expr::make_boolean(!rat.num.remainder(integer(2)).is_zero());
    }
    const auto real_val = expr->as_number_real();
    if (std::floor(real_val) != real_val)
    {
        return Expr::FALSE;
    }
    return Expr::make_boolean(static_cast<int64_t>(real_val) % 2 != 0);
}

// max
shared_ptr<Expr> primitives::max(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty())
    {
        throw GlomError("Invalid number of arguments max: at least one argument required");
    }
    const auto first_expr = eval(context, std::move(args->car()));
    if (!first_expr->is_number())
    {
        throw GlomError("Invalid argument max: " + first_expr->to_string() + " is not a number");
    }
    const auto rest = args->cdr()->as_pair();
    auto first = first_expr;
    for (const auto it: *rest)
    {
        if (!it) break;
        auto next_expr = eval(context, it);
        if (!next_expr->is_number())
        {
            throw GlomError("Invalid argument max: " + next_expr->to_string() + " is not a number");
        }
        if (primitives_utils::generic_num_gt(next_expr,first))
        {
            first = next_expr;
        }
    }
    return first;
}

// min
shared_ptr<Expr> primitives::min(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty())
    {
        throw GlomError("Invalid number of arguments min: at least one argument required");
    }
    const auto first_expr = eval(context, std::move(args->car()));
    if (!first_expr->is_number())
    {
        throw GlomError("Invalid argument min: " + first_expr->to_string() + " is not a number");
    }
    const auto rest = args->cdr()->as_pair();
    auto first = first_expr;
    for (const auto it: *rest)
    {
        if (!it) break;
        auto next_expr = eval(context, it);
        if (!next_expr->is_number())
        {
            throw GlomError("Invalid argument min: " + next_expr->to_string() + " is not a number");
        }
        if (primitives_utils::generic_num_lt(next_expr,first))
        {
            first = next_expr;
        }
    }
    return first;
}

// abs
shared_ptr<Expr> primitives::abs(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty() || !args->cdr()->is_nil())
    {
        throw GlomError("Invalid number of arguments abs: exactly one argument required");
    }
    const auto expr = eval(context, std::move(args->car()));
    if (!expr->is_number())
    {
        throw GlomError("Invalid argument abs: " + expr->to_string() + " is not a number");
    }
    if (expr->is_number_int())
    {
        return Expr::make_number_int(expr->as_number_int().abs());
    }
    if (expr->is_number_rat())
    {
        return Expr::make_number_rat(expr->as_number_rat().abs());
    }
    return Expr::make_number_real(std::fabs(expr->as_number_real()));
}

// gcd
shared_ptr<Expr> primitives::gcd(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty())
    {
        throw GlomError("Invalid number of arguments gcd: at least one argument required");
    }
    const auto first_expr = eval(context, std::move(args->car()));
    if (!first_expr->is_number_int())
    {
        throw GlomError("Invalid argument gcd: " + first_expr->to_string() + " is not an integer");
    }
    const auto rest = args->cdr()->as_pair();
    auto result = first_expr->as_number_int().abs();
    for (const auto it: *rest)
    {
        if (!it) break;
        const auto next_expr = eval(context, it);
        if (!next_expr->is_number_int())
        {
            throw GlomError("Invalid argument gcd: " + next_expr->to_string() + " is not an integer");
        }
        const auto common = rational::gcd(result, next_expr->as_number_int().abs());
        result = common;
    }
    return Expr::make_number_int(result);
}

// lcm
shared_ptr<Expr> primitives::lcm(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty())
    {
        throw GlomError("Invalid number of arguments lcm: at least one argument required");
    }
    const auto first_expr = eval(context, std::move(args->car()));
    if (!first_expr->is_number_int())
    {
        throw GlomError("Invalid argument lcm: " + first_expr->to_string() + " is not an integer");
    }
    const auto rest = args->cdr()->as_pair();
    auto result = first_expr->as_number_int().abs();
    for (const auto it: *rest)
    {
        if (!it) break;
        const auto next_expr = eval(context, it);
        if (!next_expr->is_number_int())
        {
            throw GlomError("Invalid argument lcm: " + next_expr->to_string() + " is not an integer");
        }
        if (result.is_zero() || next_expr->as_number_int().is_zero())
        {
            result = integer(0);
        }
        else
        {
            const auto common = rational::gcd(result, next_expr->as_number_int().abs());
            result = result / common * next_expr->as_number_int().abs();
        }
    }
    return Expr::make_number_int(result);
}

// floor
shared_ptr<Expr> primitives::floor(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty() || !args->cdr()->is_nil())
    {
        throw GlomError("Invalid number of arguments abs: exactly one argument required");
    }
    const auto expr = eval(context, std::move(args->car()));
    if (!expr->is_number())
    {
        throw GlomError("Invalid argument abs: " + expr->to_string() + " is not a number");
    }
    integer result;
    if (expr->is_number_int())
    {
       result = expr->as_number_int().abs();
    }
    else if (expr->is_number_rat())
    {
        result = expr->as_number_rat().to_integer();
    }
    else
    {
        return Expr::make_number_real(std::floor(expr->as_number_real()));
    }
    return Expr::make_number_int(result);
}

//ceiling
shared_ptr<Expr> primitives::ceiling(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty() || !args->cdr()->is_nil())
    {
        throw GlomError("Invalid number of arguments ceiling: exactly one argument required");
    }
    const auto expr = eval(context, std::move(args->car()));
    if (!expr->is_number())
    {
        throw GlomError("Invalid argument ceiling: " + expr->to_string() + " is not a number");
    }
    integer result;
    if (expr->is_number_int())
    {
       result = expr->as_number_int().abs();
    }
    else if (expr->is_number_rat())
    {
        result = expr->as_number_rat().to_integer();
        if (!expr->as_number_rat().is_integer() && expr->as_number_rat().is_positive())
        {
            result = result + integer(1);
        }
    }
    else
    {
        return Expr::make_number_real(std::ceil(expr->as_number_real()));
    }
    return Expr::make_number_int(result);
}

// truncate
shared_ptr<Expr> primitives::truncate(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty() || !args->cdr()->is_nil())
    {
        throw GlomError("Invalid number of arguments truncate: exactly one argument required");
    }
    const auto expr = eval(context, std::move(args->car()));
    if (!expr->is_number())
    {
        throw GlomError("Invalid argument truncate: " + expr->to_string() + " is not a number");
    }
    integer result;
    if (expr->is_number_int())
    {
        result = expr->as_number_int().abs();
    }
    else if (expr->is_number_rat())
    {
        result = expr->as_number_rat().to_integer();
    }
    else
    {
       return Expr::make_number_real(expr->as_number_real() > 0 ? std::floor(expr->as_number_real()) : std::ceil(expr->as_number_real()));
    }
    return Expr::make_number_int(result);
}

//round
shared_ptr<Expr> primitives::round(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty() || !args->cdr()->is_nil())
    {
        throw GlomError("Invalid number of arguments round: exactly one argument required");
    }
    const auto expr = eval(context, std::move(args->car()));
    if (!expr->is_number())
    {
        throw GlomError("Invalid argument round: " + expr->to_string() + " is not a number");
    }
    integer result;
    if (expr->is_number_int())
    {
        result = expr->as_number_int().abs();
    }
    else if (expr->is_number_rat())
    {
        const auto rat = expr->as_number_rat();
        const auto integer_part = rat.to_integer();
        if (const rational fractional_part = rat - rational(integer_part); fractional_part.is_zero())
        {
            result = integer_part;
        }
        else
        {
            const auto half = rational(1, 2);
            if (fractional_part.is_positive())
            {
                if (fractional_part >= half)
                {
                    result = integer_part + integer(1);
                }
                else
                {
                    result = integer_part;
                }
            }
            else
            {
                if (fractional_part <= -half)
                {
                    result = integer_part - integer(1);
                }
                else
                {
                    result = integer_part;
                }
            }
        }
    }
    else
    {
        return Expr::make_number_real(std::round(expr->as_number_real()));
    }
    return Expr::make_number_int(result);
}