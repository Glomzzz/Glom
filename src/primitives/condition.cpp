//
// Created by glom on 9/27/25.
//

#include "error.h"
#include "context.h"
#include "expr.h"
#include "primitive.h"

shared_ptr<Expr> primitives::cond_if(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> cond, then, otherwise = nullptr;
    primitives_utils::expect_2_or_3_args("if", args, cond, then, otherwise);
    if (const auto evaluated_cond = eval(context, cond); evaluated_cond->to_boolean())
    {
        return make_continuation(context, Pair::single(std::move(then)));
    }
    if (otherwise != nullptr)
    {
        return make_continuation(context, Pair::single(std::move(otherwise)));
    }
    return Expr::NOTHING;
}
shared_ptr<Expr> primitives::cond(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    auto current = args->begin();
    while (*current != nullptr)
    {
        const auto clause = std::move(*current++);
        if (!clause->is_pair())
        {
            throw GlomError("Invalid clause in cond: " + clause->to_string());
        }
        const auto& clause_pair = clause->as_pair();
        if (clause_pair->empty())
        {
            throw GlomError("Invalid syntax of clause in cond");
        }
        const auto condition = std::move(clause_pair->car());
        bool test = false;
        shared_ptr<Expr> evaluated_condition = nullptr;
        if (condition->is_symbol() && condition->as_symbol() == "else")
        {
            if (*current != nullptr)
            {
                throw GlomError("Invalid else clause in cond: else must be the last clause");
            }
            test = true;
        }
        else
        {
            evaluated_condition = eval(context, condition);
            test = evaluated_condition->to_boolean();
        }
        if (!test)
        {
            continue;
        }
        if (clause_pair->cdr()->is_nil())
        {
            return evaluated_condition;
        }
        auto body_expr = clause_pair->cdr();
        shared_ptr<Pair> body = nullptr;
        if (body_expr->is_pair())
        {
            body = body_expr->as_pair();
        }
        else
        {
            body = Pair::single(std::move(body_expr));
        }
        return make_continuation(context, std::move(body));
    }
    return Expr::NOTHING;
}