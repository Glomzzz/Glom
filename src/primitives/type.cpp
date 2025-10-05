//
// Created by glom on 9/27/25.
//

#include <cmath>

#include "error.h"
#include "context.h"
#include "expr.h"
#include "primitive.h"
#include "tokenizer.h"

shared_ptr<Expr> primitives::is_pair(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> expr = nullptr;
    primitives_utils::expect_1_arg("pair?", args, expr);
    expr = eval(context, std::move(expr));
    return Expr::make_boolean(expr->is_pair());
}
shared_ptr<Expr> primitives::is_number(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> expr = nullptr;
    primitives_utils::expect_1_arg("number?", args, expr);
    expr = eval(context, std::move(expr));
    return Expr::make_boolean(expr->is_number());
}

// is_boolean
shared_ptr<Expr> primitives::is_boolean(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> expr = nullptr;
    primitives_utils::expect_1_arg("boolean?", args, expr);
    expr = eval(context, std::move(expr));
    return Expr::make_boolean(expr->is_boolean());
}
// is_symbol
shared_ptr<Expr> primitives::is_symbol(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> expr = nullptr;
    primitives_utils::expect_1_arg("symbol?", args, expr);
    expr = eval(context, std::move(expr));
    return Expr::make_boolean(expr->is_symbol());
}

// is_string
shared_ptr<Expr> primitives::is_string(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> expr = nullptr;
    primitives_utils::expect_1_arg("string?", args, expr);
    expr = eval(context, std::move(expr));
    return Expr::make_boolean(expr->is_string());
}

// is_exact
shared_ptr<Expr> primitives::is_exact(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> expr = nullptr;
    primitives_utils::expect_1_arg("string?", args, expr);
    expr = eval(context, std::move(expr));
    return Expr::make_boolean(expr->is_number() && (expr->is_number_int() || expr->is_number_rat()));
}

// is_inexact
shared_ptr<Expr> primitives::is_inexact(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> expr = nullptr;
    primitives_utils::expect_1_arg("inexact?", args, expr);
    expr = eval(context, std::move(expr));
    return Expr::make_boolean(expr->is_number_real());
}

// exact->inexact
shared_ptr<Expr> primitives::exact_to_inexact(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> expr = nullptr;
    primitives_utils::expect_1_arg("exact->inexact", args, expr);
    expr = eval(context, std::move(expr));
    if (!expr->is_number())
    {
        throw GlomError("Invalid argument exact->inexact: " + expr->to_string() + " is not a number");
    }
    if (expr->is_number_real())
    {
        return expr;
    }
    if (expr->is_number_int())
    {
        return Expr::make_number_real(expr->as_number_int().to_real());
    }
    return Expr::make_number_real(expr->as_number_rat().to_inexact());
}

shared_ptr<Expr> primitives::inexact_to_exact(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> expr = nullptr;
    primitives_utils::expect_1_arg("inexact->exact", args, expr);
    expr = eval(context, std::move(expr));
    if (!expr->is_number())
    {
        throw GlomError("Invalid argument inexact->exact: " + expr->to_string() + " is not a number");
    }
    if (expr->is_number_int())
    {
        return Expr::make_number_real(expr->as_number_int().to_real());
    }
    if (expr->is_number_rat())
    {
        return Expr::make_number_real(expr->as_number_rat().to_inexact());
    }
    const auto real = expr->as_number_real();
    if (std::trunc(real) == real)
    {
        return Expr::make_number_int(integer(static_cast<int64_t>(real)));
    }
    return Expr::make_number_exact(rational::from_real(real));
}

// number->string
shared_ptr<Expr> primitives::number_to_string(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> expr = nullptr;
    primitives_utils::expect_1_arg("number->string", args, expr);
    expr = eval(context, std::move(expr));
    if (!expr->is_number())
    {
        throw GlomError("Invalid argument number->string: " + expr->to_string() + " is not a number");
    }
    if (expr->is_number_int())
    {
        return Expr::make_string(std::make_unique<string>(expr->as_number_int().to_decimal_string()));
    }
    if (expr->is_number_rat())
    {
        return Expr::make_string(std::make_unique<string>(expr->as_number_rat().to_rational_string()));
    }
    return Expr::make_string(std::make_unique<string>(std::to_string(expr->as_number_real())));
}

// string->number
shared_ptr<Expr> primitives::string_to_number(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> expr = nullptr;
    primitives_utils::expect_1_arg("string->number", args, expr);
    expr = eval(context, std::move(expr));
    if (!expr->is_string())
    {
        throw GlomError("Invalid argument string->number: " + expr->to_string() + " is not a string");
    }
    auto& str = expr->as_string();
    auto tokenizer = Tokenizer(str);
    const auto token = tokenizer.next();
    if (const auto next = tokenizer.next(); next.get_type() != TOKEN_EOI)
    {
        return Expr::FALSE;
    }
    if (token.get_type() == TOKEN_NUMBER_INT)
    {
        return Expr::make_number_int(token.as_number_int());
    }
    if (token.get_type() == TOKEN_NUMBER_RAT)
    {
        return Expr::make_number_rat(token.as_number_rat());
    }
    if (token.get_type() == TOKEN_NUMBER_REAL)
    {
        return Expr::make_number_real(token.as_number_real());
    }
    return Expr::FALSE;
}

// symbol->string
shared_ptr<Expr> primitives::symbol_to_string(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> expr = nullptr;
    primitives_utils::expect_1_arg("symbol->string", args, expr);
    expr = eval(context, std::move(expr));
    if (!expr->is_symbol())
    {
        throw GlomError("Invalid argument symbol->string: " + expr->to_string() + " is not a symbol");
    }
    return Expr::make_string(std::make_unique<string>(expr->as_symbol()));
}

// string->symbol
shared_ptr<Expr> primitives::string_to_symbol(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> expr = nullptr;
    primitives_utils::expect_1_arg("string->symbol", args, expr);
    expr = eval(context, std::move(expr));
    if (!expr->is_string())
    {
        throw GlomError("Invalid argument string->symbol: " + expr->to_string() + " is not a string");
    }
    return Expr::make_symbol(expr->as_string());
}


shared_ptr<Expr> primitives::eq_string(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty() || args->cdr()->is_nil())
    {
        throw GlomError("Invalid number of arguments string=?: at least two arguments required");
    }
    const auto first_expr = eval(context, std::move(args->car()));
    if (!first_expr->is_string())
    {
        throw GlomError("Invalid argument string=?: " + first_expr->to_string() + " is not a string");
    }
    const auto rest = args->cdr()->as_pair();
    const auto& first = first_expr;
    for (const auto it: *rest)
    {
        if (!it) break;
        const auto next_expr = eval(context, it);
        if (!next_expr->is_string())
        {
            throw GlomError("Invalid argument string=?: " + next_expr->to_string() + " is not a string");
        }
        if (first->as_string() != next_expr->as_string())
        {
            return Expr::FALSE;
        }
    }
    return Expr::TRUE;
}

shared_ptr<Expr> primitives::eq_string_ignore_case(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty() || args->cdr()->is_nil())
    {
        throw GlomError("Invalid number of arguments string-ci=?: at least two arguments required");
    }
    const auto first_expr = eval(context, std::move(args->car()));
    if (!first_expr->is_string())
    {
        throw GlomError("Invalid argument string-ci=?: " + first_expr->to_string() + " is not a string");
    }
    const auto rest = args->cdr()->as_pair();
    const auto& first = first_expr;
    for (const auto it: *rest)
    {
        if (!it) break;
        const auto next_expr = eval(context, it);
        if (!next_expr->is_string())
        {
            throw GlomError("Invalid argument string-ci=?: " + next_expr->to_string() + " is not a string");
        }

        // case insensitive comparison
        const auto& str1 = first->as_string();
        const auto& str2 = next_expr->as_string();
        if (str1.size() != str2.size()) return Expr::FALSE;
        for (size_t i = 0; i < str1.size(); ++i)
        {
            if (tolower(str1[i]) != tolower(str2[i])) return Expr::FALSE;
        }
    }
    return Expr::TRUE;
}
