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
    expr = eval(context, expr);
    return Expr::make_boolean(expr->is_nil());
}
shared_ptr<Expr> primitives::cons(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> a, b = nullptr;
    primitives_utils::expect_2_args("cons", args, a, b);
    a = eval(context, a);
    b = eval(context, b);
    return Expr::make_pair(Pair::cons(a,b));
}
shared_ptr<Expr> primitives::car(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> pair_expr = nullptr;
    primitives_utils::expect_1_arg("car", args, pair_expr);
    pair_expr = eval(context, pair_expr);
    shared_ptr<Expr> car_expr = nullptr;
    primitives_utils::take_car("car", pair_expr, pair_expr, car_expr);
    return car_expr;
}
shared_ptr<Expr> primitives::caar(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> pair_expr = nullptr;
    primitives_utils::expect_1_arg("caar", args, pair_expr);
    pair_expr = eval(context, pair_expr);
    shared_ptr<Expr> rest = pair_expr;
    primitives_utils::take_car("caar", pair_expr, rest, rest);
    primitives_utils::take_car("caar", pair_expr, rest, rest);
    return rest;
}
shared_ptr<Expr> primitives::caaar(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> pair_expr = nullptr;
    primitives_utils::expect_1_arg("caaar", args, pair_expr);
    pair_expr = eval(context, pair_expr);
    shared_ptr<Expr> rest = pair_expr;
    primitives_utils::take_car("caaar", pair_expr, rest, rest);
    primitives_utils::take_car("caaar", pair_expr, rest, rest);
    primitives_utils::take_car("caaar", pair_expr, rest, rest);
    return rest;
}
shared_ptr<Expr> primitives::caaaar(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> pair_expr = nullptr;
    primitives_utils::expect_1_arg("caaaar", args, pair_expr);
    pair_expr = eval(context, pair_expr);
    shared_ptr<Expr> rest = pair_expr;
    primitives_utils::take_car("caaaar", pair_expr, rest, rest);
    primitives_utils::take_car("caaaar", pair_expr, rest, rest);
    primitives_utils::take_car("caaaar", pair_expr, rest, rest);
    primitives_utils::take_car("caaaar", pair_expr, rest, rest);
    return rest;
}
shared_ptr<Expr> primitives::caaadr(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> pair_expr = nullptr;
    primitives_utils::expect_1_arg("caaadr", args, pair_expr);
    pair_expr = eval(context, pair_expr);
    shared_ptr<Expr> rest = pair_expr;
    primitives_utils::take_cdr("caaadr", pair_expr, rest, rest);
    primitives_utils::take_car("caaadr", pair_expr, rest, rest);
    primitives_utils::take_car("caaadr", pair_expr, rest, rest);
    primitives_utils::take_car("caaadr", pair_expr, rest, rest);
    return rest;
}
shared_ptr<Expr> primitives::caadr(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> pair_expr = nullptr;
    primitives_utils::expect_1_arg("caadr", args, pair_expr);
    pair_expr = eval(context, pair_expr);
    shared_ptr<Expr> rest = pair_expr;
    primitives_utils::take_cdr("caadr", pair_expr, rest, rest);
    primitives_utils::take_car("caadr", pair_expr, rest, rest);
    primitives_utils::take_car("caadr", pair_expr, rest, rest);
    return rest;
}
shared_ptr<Expr> primitives::caadar(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> pair_expr = nullptr;
    primitives_utils::expect_1_arg("caadar", args, pair_expr);
    pair_expr = eval(context, pair_expr);
    shared_ptr<Expr> rest = pair_expr;
    primitives_utils::take_car("caadar", pair_expr, rest, rest);
    primitives_utils::take_cdr("caadar", pair_expr, rest, rest);
    primitives_utils::take_car("caadar", pair_expr, rest, rest);
    primitives_utils::take_car("caadar", pair_expr, rest, rest);
    return rest;
}
shared_ptr<Expr> primitives::caaddr(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> pair_expr = nullptr;
    primitives_utils::expect_1_arg("caaddr", args, pair_expr);
    pair_expr = eval(context, pair_expr);
    shared_ptr<Expr> rest = pair_expr;
    primitives_utils::take_cdr("caaddr", pair_expr, rest, rest);
    primitives_utils::take_cdr("caaddr", pair_expr, rest, rest);
    primitives_utils::take_car("caaddr", pair_expr, rest, rest);
    primitives_utils::take_car("caaddr", pair_expr, rest, rest);
    return rest;
}
shared_ptr<Expr> primitives::cadr(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> pair_expr = nullptr;
    primitives_utils::expect_1_arg("cadr", args, pair_expr);
    pair_expr = eval(context, pair_expr);
    shared_ptr<Expr> rest = pair_expr;
    primitives_utils::take_cdr("cadr", pair_expr, rest, rest);
    primitives_utils::take_car("cadr", pair_expr, rest, rest);
    return rest;
}
shared_ptr<Expr> primitives::cadar(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> pair_expr = nullptr;
    primitives_utils::expect_1_arg("cadar", args, pair_expr);
    pair_expr = eval(context, pair_expr);
    shared_ptr<Expr> rest = pair_expr;
    primitives_utils::take_car("cadar", pair_expr, rest, rest);
    primitives_utils::take_cdr("cadar", pair_expr, rest, rest);
    primitives_utils::take_car("cadar", pair_expr, rest, rest);
    return rest;
}
shared_ptr<Expr> primitives::cadaar(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> pair_expr = nullptr;
    primitives_utils::expect_1_arg("cadaar", args, pair_expr);
    pair_expr = eval(context, pair_expr);
    shared_ptr<Expr> rest = pair_expr;
    primitives_utils::take_car("cadaar", pair_expr, rest, rest);
    primitives_utils::take_car("cadaar", pair_expr, rest, rest);
    primitives_utils::take_cdr("cadaar", pair_expr, rest, rest);
    primitives_utils::take_car("cadaar", pair_expr, rest, rest);
    return rest;
}
shared_ptr<Expr> primitives::cadadr(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> pair_expr = nullptr;
    primitives_utils::expect_1_arg("cadadr", args, pair_expr);
    pair_expr = eval(context, pair_expr);
    shared_ptr<Expr> rest = pair_expr;
    primitives_utils::take_cdr("cadadr", pair_expr, rest, rest);
    primitives_utils::take_car("cadadr", pair_expr, rest, rest);
    primitives_utils::take_cdr("cadadr", pair_expr, rest, rest);
    primitives_utils::take_car("cadadr", pair_expr, rest, rest);
    return rest;
}
shared_ptr<Expr> primitives::caddr(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> pair_expr = nullptr;
    primitives_utils::expect_1_arg("caddr", args, pair_expr);
    pair_expr = eval(context, pair_expr);
    shared_ptr<Expr> rest = pair_expr;
    primitives_utils::take_cdr("caddr", pair_expr, rest, rest);
    primitives_utils::take_cdr("caddr", pair_expr, rest, rest);
    primitives_utils::take_car("caddr", pair_expr, rest, rest);
    return rest;
}
shared_ptr<Expr> primitives::caddar(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> pair_expr = nullptr;
    primitives_utils::expect_1_arg("caddar", args, pair_expr);
    pair_expr = eval(context, pair_expr);
    shared_ptr<Expr> rest = pair_expr;
    primitives_utils::take_car("caddar", pair_expr, rest, rest);
    primitives_utils::take_cdr("caddar", pair_expr, rest, rest);
    primitives_utils::take_cdr("caddar", pair_expr, rest, rest);
    primitives_utils::take_car("caddar", pair_expr, rest, rest);
    return rest;
}
shared_ptr<Expr> primitives::cadddr(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> pair_expr = nullptr;
    primitives_utils::expect_1_arg("cadddr", args, pair_expr);
    pair_expr = eval(context, pair_expr);
    shared_ptr<Expr> rest = pair_expr;
    primitives_utils::take_cdr("cadddr", pair_expr, rest, rest);
    primitives_utils::take_cdr("cadddr", pair_expr, rest, rest);
    primitives_utils::take_cdr("cadddr", pair_expr, rest, rest);
    primitives_utils::take_car("cadddr", pair_expr, rest, rest);
    return rest;
}

shared_ptr<Expr> primitives::cdr(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> pair_expr = nullptr;
    primitives_utils::expect_1_arg("car", args, pair_expr);
    pair_expr = eval(context, pair_expr);
    if (!pair_expr->is_pair())
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
shared_ptr<Expr> primitives::cdar(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> pair_expr = nullptr;
    primitives_utils::expect_1_arg("cdar", args, pair_expr);
    pair_expr = eval(context, pair_expr);
    shared_ptr<Expr> rest = pair_expr;
    primitives_utils::take_car("cdar", pair_expr, rest, rest);
    primitives_utils::take_cdr("cdar", pair_expr, rest, rest);
    return rest;
}
shared_ptr<Expr> primitives::cdaar(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> pair_expr = nullptr;
    primitives_utils::expect_1_arg("cdaar", args, pair_expr);
    pair_expr = eval(context, pair_expr);
    shared_ptr<Expr> rest = pair_expr;
    primitives_utils::take_car("cdaar", pair_expr, rest, rest);
    primitives_utils::take_car("cdaar", pair_expr, rest, rest);
    primitives_utils::take_cdr("cdaar", pair_expr, rest, rest);
    return rest;
}
shared_ptr<Expr> primitives::cdaaar(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> pair_expr = nullptr;
    primitives_utils::expect_1_arg("cdaaar", args, pair_expr);
    pair_expr = eval(context, pair_expr);
    shared_ptr<Expr> rest = pair_expr;
    primitives_utils::take_car("cdaaar", pair_expr, rest, rest);
    primitives_utils::take_car("cdaaar", pair_expr, rest, rest);
    primitives_utils::take_car("cdaaar", pair_expr, rest, rest);
    primitives_utils::take_cdr("cdaaar", pair_expr, rest, rest);
    return rest;
}
shared_ptr<Expr> primitives::cdaadr(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> pair_expr = nullptr;
    primitives_utils::expect_1_arg("cdaadr", args, pair_expr);
    pair_expr = eval(context, pair_expr);
    shared_ptr<Expr> rest = pair_expr;
    primitives_utils::take_cdr("cdaadr", pair_expr, rest, rest);
    primitives_utils::take_car("cdaadr", pair_expr, rest, rest);
    primitives_utils::take_car("cdaadr", pair_expr, rest, rest);
    primitives_utils::take_cdr("cdaadr", pair_expr, rest, rest);
    return rest;
}
shared_ptr<Expr> primitives::cdadr(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> pair_expr = nullptr;
    primitives_utils::expect_1_arg("cdadr", args, pair_expr);
    pair_expr = eval(context, pair_expr);
    shared_ptr<Expr> rest = pair_expr;
    primitives_utils::take_cdr("cdadr", pair_expr, rest, rest);
    primitives_utils::take_car("cdadr", pair_expr, rest, rest);
    primitives_utils::take_cdr("cdadr", pair_expr, rest, rest);
    return rest;
}
shared_ptr<Expr> primitives::cdadar(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> pair_expr = nullptr;
    primitives_utils::expect_1_arg("cdadar", args, pair_expr);
    pair_expr = eval(context, pair_expr);
    shared_ptr<Expr> rest = pair_expr;
    primitives_utils::take_car("cdadar", pair_expr, rest, rest);
    primitives_utils::take_cdr("cdadar", pair_expr, rest, rest);
    primitives_utils::take_car("cdadar", pair_expr, rest, rest);
    primitives_utils::take_cdr("cdadar", pair_expr, rest, rest);
    return rest;
}
shared_ptr<Expr> primitives::cdaddr(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> pair_expr = nullptr;
    primitives_utils::expect_1_arg("cdaddr", args, pair_expr);
    pair_expr = eval(context, pair_expr);
    shared_ptr<Expr> rest = pair_expr;
    primitives_utils::take_cdr("cdaddr", pair_expr, rest, rest);
    primitives_utils::take_cdr("cdaddr", pair_expr, rest, rest);
    primitives_utils::take_car("cdaddr", pair_expr, rest, rest);
    primitives_utils::take_cdr("cdaddr", pair_expr, rest, rest);
    return rest;
}
shared_ptr<Expr> primitives::cddr(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> pair_expr = nullptr;
    primitives_utils::expect_1_arg("cddr", args, pair_expr);
    pair_expr = eval(context, pair_expr);
    shared_ptr<Expr> rest = pair_expr;
    primitives_utils::take_cdr("cddr", pair_expr, rest, rest);
    primitives_utils::take_cdr("cddr", pair_expr, rest, rest);
    return rest;
}
shared_ptr<Expr> primitives::cddar(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> pair_expr = nullptr;
    primitives_utils::expect_1_arg("cddar", args, pair_expr);
    pair_expr = eval(context, pair_expr);
    shared_ptr<Expr> rest = pair_expr;
    primitives_utils::take_car("cddar", pair_expr, rest, rest);
    primitives_utils::take_cdr("cddar", pair_expr, rest, rest);
    primitives_utils::take_cdr("cddar", pair_expr, rest, rest);
    return rest;
}
shared_ptr<Expr> primitives::cddaar(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> pair_expr = nullptr;
    primitives_utils::expect_1_arg("cddaar", args, pair_expr);
    pair_expr = eval(context, pair_expr);
    shared_ptr<Expr> rest = pair_expr;
    primitives_utils::take_car("cddaar", pair_expr, rest, rest);
    primitives_utils::take_car("cddaar", pair_expr, rest, rest);
    primitives_utils::take_cdr("cddaar", pair_expr, rest, rest);
    primitives_utils::take_cdr("cddaar", pair_expr, rest, rest);
    return rest;
}
shared_ptr<Expr> primitives::cddadr(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> pair_expr = nullptr;
    primitives_utils::expect_1_arg("cddadr", args, pair_expr);
    pair_expr = eval(context, pair_expr);
    shared_ptr<Expr> rest = pair_expr;
    primitives_utils::take_cdr("cddadr", pair_expr, rest, rest);
    primitives_utils::take_car("cddadr", pair_expr, rest, rest);
    primitives_utils::take_cdr("cddadr", pair_expr, rest, rest);
    primitives_utils::take_cdr("cddadr", pair_expr, rest, rest);
    return rest;
}
shared_ptr<Expr> primitives::cdddr(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> pair_expr = nullptr;
    primitives_utils::expect_1_arg("cdddr", args, pair_expr);
    pair_expr = eval(context, pair_expr);
    shared_ptr<Expr> rest = pair_expr;
    primitives_utils::take_cdr("cdddr", pair_expr, rest, rest);
    primitives_utils::take_cdr("cdddr", pair_expr, rest, rest);
    primitives_utils::take_cdr("cdddr", pair_expr, rest, rest);
    return rest;
}
shared_ptr<Expr> primitives::cdddar(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> pair_expr = nullptr;
    primitives_utils::expect_1_arg("cdddar", args, pair_expr);
    pair_expr = eval(context, pair_expr);
    shared_ptr<Expr> rest = pair_expr;
    primitives_utils::take_car("cdddar", pair_expr, rest, rest);
    primitives_utils::take_cdr("cdddar", pair_expr, rest, rest);
    primitives_utils::take_cdr("cdddar", pair_expr, rest, rest);
    primitives_utils::take_cdr("cdddar", pair_expr, rest, rest);
    return rest;
}
shared_ptr<Expr> primitives::cddddr(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> pair_expr = nullptr;
    primitives_utils::expect_1_arg("cddddr", args, pair_expr);
    pair_expr = eval(context, pair_expr);
    shared_ptr<Expr> rest = pair_expr;
    primitives_utils::take_cdr("cddddr", pair_expr, rest, rest);
    primitives_utils::take_cdr("cddddr", pair_expr, rest, rest);
    primitives_utils::take_cdr("cddddr", pair_expr, rest, rest);
    primitives_utils::take_cdr("cddddr", pair_expr, rest, rest);
    return rest;
}

shared_ptr<Expr> primitives::list(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Pair> list = nullptr;
    shared_ptr<Pair> tail = nullptr;
    for (auto elem_expr : *args)
    {
        if (!elem_expr) break;
        auto last = eval(context, std::move(elem_expr));
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

//append
shared_ptr<Expr> primitives::append(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Pair> result = nullptr;
    shared_ptr<Pair> tail = nullptr;
    for (auto list_expr : *args)
    {
        if (!list_expr) break;
        const auto list = eval(context, std::move(list_expr));
        if (list->is_nil())
        {
            continue;
        }
        if (!list->is_pair())
        {
            throw GlomError("append: argument is not a list: " + list->to_string());
        }
        auto current = list->as_pair()->begin();
        while (*current != nullptr)
        {
            auto elem = std::move(*current++);
            if (!result)
            {
                result = Pair::single(std::move(elem));
                tail = result;
            }
            else
            {
                const auto new_tail = Pair::single(std::move(elem));
                tail->set_cdr(Expr::make_pair(new_tail));
                tail = new_tail;
            }
        }
    }
    if (!result)
        return Expr::NIL;
    return Expr::make_pair(result);
}

// length
shared_ptr<Expr> primitives::length(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> list_expr = nullptr;
    primitives_utils::expect_1_arg("length", args, list_expr);
    list_expr = eval(context, list_expr);
    if (list_expr->is_nil())
    {
        return Expr::make_number_int(integer(0));
    }
    if (!list_expr->is_pair())
    {
        throw GlomError("length: argument is not a list: " + list_expr->to_string());
    }
    const auto list = list_expr->as_pair();
    int64_t len = 0;
    for (const auto& _ : *list)
    {
        if (!_ ) break;
        len++;
    }
    return Expr::make_number_int(integer(len));
}