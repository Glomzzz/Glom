//
// Created by glom on 9/29/25.
//

#include "error.h"
#include "primitive.h"
#include "expr.h"

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

void primitives_utils::expect_2_or_3_args(const string& name, const shared_ptr<Pair>& args, shared_ptr<Expr>& a, shared_ptr<Expr>& b, shared_ptr<Expr>& c)
{
    if (args->empty())
    {
        throw GlomError(name + ": expects 2 or 3 arguments, given 0");
    }
    a = args->car();
    const auto cdr = args->cdr();
    if (cdr->is_nil())
    {
        throw GlomError(name + ": expects 2 or 3 arguments, given 1");
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
        throw GlomError(name + ": expects 2 or 3 arguments, given more than 3");
    }
}
