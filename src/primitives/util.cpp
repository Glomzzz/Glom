//
// Created by glom on 9/29/25.
//

#include "error.h"
#include "primitive.h"
#include "expr.h"

void primitives_utils::coerce_number(shared_ptr<Expr>& a, shared_ptr<Expr>& b)
{
    if (!a->is_number() || !b->is_number())
    {
        throw GlomError("Invalid argument +: both arguments must be numbers");
    }
    if (a->get_type() == b->get_type())
    {
        return;
    }
    if (a->is_number_real() || b->is_number_real())
    {
        if (!a->is_number_real())
            a = Expr::make_number_real(a->to_number_real());
        if (!b->is_number_real())
            b = Expr::make_number_real(b->to_number_real());
        return;
    }
    if (a->is_number_rat() || b->is_number_rat())
    {
        if (!a->is_number_rat())
            a = Expr::make_number_rat(a->to_number_rat());
        if (!b->is_number_rat())
            b = Expr::make_number_rat(b->to_number_rat());
    }
}

void primitives_utils::expect_1_arg(const string& proc, const shared_ptr<Pair>& args, shared_ptr<Expr>& a)
{
    if (args->empty())
    {
        throw GlomError(proc + ": expects 1 argument, given 0");
    }
    const auto car = args->car();
    if (!args->cdr()->is_nil())
    {
        throw GlomError(proc + ": expects 1 argument, given more than 1");
    }
    a = car;
}

void primitives_utils::expect_2_args(const string& proc, const shared_ptr<Pair>& args, shared_ptr<Expr>& a, shared_ptr<Expr>& b)
{
    if (args->empty())
    {
        throw GlomError(proc + ": expects 2 arguments, given 0");
    }
    a = args->car();
    const auto cdr = args->cdr();
    if (cdr->is_nil())
    {
        throw GlomError(proc + ": expects 2 arguments, given 1");
    }
    const auto rest = cdr->as_pair();
    b = rest->car();
    if (!rest->cdr()->is_nil())
    {
        throw GlomError(proc + ": expects 2 arguments, given more than 2");
    }
}

void primitives_utils::expect_2_or_3_args(const string& proc, const shared_ptr<Pair>& args, shared_ptr<Expr>& a, shared_ptr<Expr>& b, shared_ptr<Expr>& c)
{
    if (args->empty())
    {
        throw GlomError(proc + ": expects 2 or 3 arguments, given 0");
    }
    a = args->car();
    const auto cdr = args->cdr();
    if (cdr->is_nil())
    {
        throw GlomError(proc + ": expects 2 or 3 arguments, given 1");
    }
    const auto rest = cdr->as_pair();
    b = rest->car();
    const auto rest_cdr = rest->cdr();
    if (rest_cdr->is_nil())
    {
        c = nullptr;
        return;
    }
    const auto rest2 = rest_cdr->as_pair();
    c = rest2->car();
    if (!rest2->cdr()->is_nil())
    {
        throw GlomError(proc + ": expects 2 or 3 arguments, given more than 3");
    }
}

void primitives_utils::take_car(const string& proc, const shared_ptr<Expr>& list, const shared_ptr<Expr>& expr, shared_ptr<Expr>& car)
{
    if (!expr->is_pair() || expr->is_nil())
    {
        throw GlomError(proc + ": incorrect list structure: " + list->to_string());
    }
    const auto pair = expr->as_pair();
    car = pair->car();
}
void primitives_utils::take_cdr(const string& proc, const shared_ptr<Expr>& list, const shared_ptr<Expr>& expr, shared_ptr<Expr>& cdr)
{
    if (!expr->is_pair() || expr->is_nil())
    {
        throw GlomError(proc + ": incorrect list structure: " + list->to_string());
    }
    const auto pair = expr->as_pair();
    cdr = pair->cdr();
}
