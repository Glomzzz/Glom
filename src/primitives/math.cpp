//
// Created by glom on 9/27/25.
//
#include <complex>

#include "error.h"
#include "expr.h"
#include "context.h"
#include "primitive.h"

shared_ptr<Expr> primitives::exponentiation(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty() || !args->cdr()->is_nil())
    {
        throw GlomError("Invalid number of arguments exp: exactly one argument required");
    }
    const auto power_expr = eval(context, std::move(args->car()));
    if (!power_expr->is_number())
    {
        throw GlomError("Invalid argument exp: " + power_expr->to_string() + " is not a number");
    }
    if (power_expr->is_number_int())
    {
        if (const auto power = power_expr->as_number_int(); power.is_zero())
        {
            return Expr::make_number_int(integer(0));
        }
    }
    const auto power = power_expr->to_number_real();
    const auto result = std::pow(M_E, power);
    return Expr::make_number_real(result);
}
shared_ptr<Expr> primitives::logarithm(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty())
    {
        throw GlomError("Invalid number of arguments log: at least 1 argument required");
    }
    const auto arg_expr = eval(context, std::move(args->car()));
    if (!arg_expr->is_number())
    {
        throw GlomError("Invalid argument log: " + arg_expr->to_string() + " is not a number");
    }
    if (arg_expr->is_number_int())
    {
        const auto arg = arg_expr->as_number_int();
        if (arg.is_zero())
        {
            throw GlomError("Invalid argument log: logarithm of zero is undefined");
        }
        if (arg == integer(1))
        {
            return Expr::make_number_int(integer(0));
        }
    }
    const auto arg = arg_expr->to_number_real();
    if (arg <= 0)
    {
        throw GlomError("Invalid argument log: logarithm of non-positive number is undefined");
    }
    args = args->cdr()->as_pair();
    shared_ptr<Expr> base_expr;
    if (!args->empty())
    {
        base_expr = eval(context, std::move(args->car()));
        if (!args->cdr()->is_nil())
        {
            throw GlomError("Invalid number of arguments log: at most 2 arguments allowed");
        }
        if (!base_expr->is_number())
        {
            throw GlomError("Invalid argument log: " + base_expr->to_string() + " is not a number");
        }
        if (base_expr->is_number_int())
        {
            if (const auto base = base_expr->as_number_int(); base.is_zero() || base == integer(1))
            {
                throw GlomError("Invalid argument log: logarithm base must be positive and not equal to 1");
            }
        }
    }
    else
    {
        base_expr = Expr::make_number_real(M_E);
    }
    const auto base = base_expr->to_number_real();
    const auto result = std::log(arg) / std::log(base);
    return Expr::make_number_real(result);
}

// sine
shared_ptr<Expr> primitives::sine(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty() || !args->cdr()->is_nil())
    {
        throw GlomError("Invalid number of arguments sin: exactly one argument required");
    }
    const auto angle_expr = eval(context, std::move(args->car()));
    if (!angle_expr->is_number())
    {
        throw GlomError("Invalid argument sin: " + angle_expr->to_string() + " is not a number");
    }
    if (angle_expr->is_number_int())
    {
        if (const auto angle = angle_expr->as_number_int(); angle.is_zero())
        {
            return Expr::make_number_int(integer(0));
        }
    }
    const auto angle = angle_expr->to_number_real();
    const auto result = std::sin(angle);
    return Expr::make_number_real(result);
}

// cosine
shared_ptr<Expr> primitives::cosine(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty() || !args->cdr()->is_nil())
    {
        throw GlomError("Invalid number of arguments cos: exactly one argument required");
    }
    const auto angle_expr = eval(context, std::move(args->car()));
    if (!angle_expr->is_number())
    {
        throw GlomError("Invalid argument cos: " + angle_expr->to_string() + " is not a number");
    }
    if (angle_expr->is_number_int())
    {
        if (const auto angle = angle_expr->as_number_int(); angle.is_zero())
        {
            return Expr::make_number_int(integer(1));
        }
    }
    const auto angle = angle_expr->to_number_real();
    const auto result = std::cos(angle);
    return Expr::make_number_real(result);
}

// tangent
shared_ptr<Expr> primitives::tangent(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty() || !args->cdr()->is_nil())
    {
        throw GlomError("Invalid number of arguments tan: exactly one argument required");
    }
    const auto angle_expr = eval(context, std::move(args->car()));
    if (!angle_expr->is_number())
    {
        throw GlomError("Invalid argument tan: " + angle_expr->to_string() + " is not a number");
    }
    if (angle_expr->is_number_int())
    {
        if (const auto angle = angle_expr->as_number_int(); angle.is_zero())
        {
            return Expr::make_number_int(integer(0));
        }
    }
    const auto angle = angle_expr->to_number_real();
    const auto result = std::tan(angle);
    return Expr::make_number_real(result);
}

// arcsine
shared_ptr<Expr> primitives::arcsine(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty() || !args->cdr()->is_nil())
    {
        throw GlomError("Invalid number of arguments asin: exactly one argument required");
    }
    const auto value_expr = eval(context, std::move(args->car()));
    if (!value_expr->is_number())
    {
        throw GlomError("Invalid argument asin: " + value_expr->to_string() + " is not a number");
    }
    if (value_expr->is_number_int())
    {
        const auto value = value_expr->as_number_int();
        if (value.is_zero())
        {
            return Expr::make_number_int(integer(0));
        }
        if (value == integer(1))
        {
            return Expr::make_number_real(M_PI / 2);
        }
        if (value == integer(-1))
        {
            return Expr::make_number_real(-M_PI / 2);
        }
    }
    const auto value = value_expr->to_number_real();
    if (value < -1 || value > 1)
    {
        throw GlomError("Invalid argument asin: arcsine is only defined for values in the range [-1, 1]");
    }
    const auto result = std::asin(value);
    return Expr::make_number_real(result);
}

// arccosine
shared_ptr<Expr> primitives::arccosine(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty() || !args->cdr()->is_nil())
    {
        throw GlomError("Invalid number of arguments acos: exactly one argument required");
    }
    const auto value_expr = eval(context, std::move(args->car()));
    if (!value_expr->is_number())
    {
        throw GlomError("Invalid argument acos: " + value_expr->to_string() + " is not a number");
    }
    if (value_expr->is_number_int())
    {
        const auto value = value_expr->as_number_int();
        if (value == integer(1))
        {
            return Expr::make_number_int(integer(0));
        }
        if (value == integer(0))
        {
            return Expr::make_number_real(M_PI / 2);
        }
        if (value == integer(-1))
        {
            return Expr::make_number_real(M_PI);
        }
    }
    const auto value = value_expr->to_number_real();
    if (value < -1 || value > 1)
    {
        throw GlomError("Invalid argument acos: arccosine is only defined for values in the range [-1, 1]");
    }
    const auto result = std::acos(value);
    return Expr::make_number_real(result);
}

// arctangent
shared_ptr<Expr> primitives::arctangent(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty())
    {
        throw GlomError("Invalid number of arguments atan: at least one argument required");
    }
    const auto y_expr = eval(context, std::move(args->car()));
    if (!y_expr->is_number())
    {
        throw GlomError("Invalid argument atan: " + y_expr->to_string() + " is not a number");
    }
    if (y_expr->is_number_int())
    {
        if (const auto y = y_expr->as_number_int(); y.is_zero())
        {
            return Expr::make_number_int(integer(0));
        }
    }
    const auto y = y_expr->to_number_real();
    args = args->cdr()->as_pair();
    shared_ptr<Expr> x_expr;
    if (!args->empty())
    {
        x_expr = eval(context, std::move(args->car()));
        if (!args->cdr()->is_nil())
        {
            throw GlomError("Invalid number of arguments atan: at most 2 arguments allowed");
        }
        if (!x_expr->is_number())
        {
            throw GlomError("Invalid argument atan: " + x_expr->to_string() + " is not a number");
        }
        if (x_expr->is_number_int())
        {
            if (const auto x = x_expr->as_number_int(); x.is_zero())
            {
                if (y > 0)
                {
                    return Expr::make_number_real(M_PI / 2);
                }
                return Expr::make_number_real(-M_PI / 2);
            }
        }
    }
    else
    {
        x_expr = Expr::make_number_int(integer(1));
    }
    const auto x = x_expr->to_number_real();
    const auto result = std::atan2(y, x);
    return Expr::make_number_real(result);
}

// square root
shared_ptr<Expr> primitives::square_root(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty() || !args->cdr()->is_nil())
    {
        throw GlomError("Invalid number of arguments sqrt: exactly one argument required");
    }
    const auto value_expr = eval(context, std::move(args->car()));
    if (!value_expr->is_number())
    {
        throw GlomError("Invalid argument sqrt: " + value_expr->to_string() + " is not a number");
    }
    if (value_expr->is_number_int())
    {
        const auto value = value_expr->as_number_int();
        if (value.is_negative())
        {
            throw GlomError("Invalid argument sqrt: cannot compute square root of negative integer");
        }
        const auto result = value.sqrt();
        if (result.index() == 0)
        {
            return Expr::make_number_int(std::get<0>(result));
        }
        return Expr::make_number_real(std::get<1>(result));
    }
    if (value_expr->is_number_rat())
    {
        const auto value = value_expr->as_number_rat();
        if (value.is_negative())
        {
            throw GlomError("Invalid argument sqrt: cannot compute square root of negative rational number");
        }
        const auto numerator_sqrt = value.numerator().sqrt();
        const auto denominator_sqrt = value.denominator().sqrt();
        if (numerator_sqrt.index() == 0 && denominator_sqrt.index() == 0)
        {
            return Expr::make_number_rat(rational(std::get<0>(numerator_sqrt),std::get<0>(denominator_sqrt)));
        }
        const auto real_numerator = numerator_sqrt.index() == 0 ? std::get<0>(numerator_sqrt).to_real() : std::get<1>(numerator_sqrt);
        const auto real_denominator = denominator_sqrt.index() == 0 ? std::get<0>(denominator_sqrt).to_real() : std::get<1>(denominator_sqrt);
        return Expr::make_number_real(real_numerator / real_denominator);
    }
    const auto value = value_expr->as_number_real();
    if (value < 0)
    {
        throw GlomError("Invalid argument sqrt: cannot compute square root of negative real number");
    }
    const auto result = std::sqrt(value);
    return Expr::make_number_real(result);
}

shared_ptr<Expr> primitives::square_root_integer(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty() || !args->cdr()->is_nil())
    {
        throw GlomError("Invalid number of arguments isqrt: exactly one argument required");
    }
    const auto value_expr = eval(context, std::move(args->car()));
    if (!value_expr->is_number_int())
    {
        throw GlomError("Invalid argument isqrt: " + value_expr->to_string() + " is not a integer");
    }
    const auto value = value_expr->as_number_int();
    if (value.is_negative())
    {
        throw GlomError("Invalid argument isqrt: cannot compute square root of negative integer");
    }
    return Expr::make_number_int(value.isqrt());
}