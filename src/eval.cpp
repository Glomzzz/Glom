//
// Created by glom on 9/26/25.
//

#include <iostream>

#include "context.h"
#include "expr.h"
#include "error.h"

shared_ptr<Expr> Context::eval(shared_ptr<Expr> expr) {
    if (!expr)
    {
        throw GlomError("Cannot evaluate null expression");
    }

    switch (expr->get_type())
    {
    case NUMBER:
    case STRING:
    case BOOLEAN:
    case LAMBDA:
    case PRIMITIVE:
        return std::move(expr);

    case SYMBOL:
        {
            const auto& name = expr->as_symbol();
            auto var = get(name);
            if (!var)
            {
                throw GlomError("Undefined variable: " + view_to_string(name));
            }
            return var;
        }

    case PAIR:
        {
            const auto pair = expr->as_pair();
            auto proc = eval(std::move(pair->car()));
            auto rest = pair->cdr();
            shared_ptr<Pair> args = nullptr;
            if (rest && rest->get_type() == PAIR)
            {
                args = rest->as_pair();
            } else if (rest)
            {
                args = Pair::single(std::move(rest));
            }

            switch (proc->get_type())
            {
            case PRIMITIVE:
                {
                    const auto primitive = proc->as_primitive();
                    return (*primitive)(shared_from_this(),std::move(args));
                }
            case LAMBDA:
                {
                    const auto lambda = proc->as_lambda();
                    const auto& params = lambda->get_params();
                    const auto& body = lambda->get_body();
                    shared_ptr<Context> apply_context = nullptr;
                    if (params.empty())
                    {
                        apply_context = lambda->get_context();
                    } else
                    {
                        apply_context = eval_apply_context(proc, lambda->get_context(), params, std::move(args));
                    }
                    return apply_context->eval(body);
                }
            default:
                throw GlomError( proc->to_string() + " is not a procedure: " + expr->to_string());
            }
        }
    default:
        throw GlomError("Unsupported expression type");
    }
}


bool Expr::to_boolean() const
{
    if (get_type() != BOOLEAN)
    {
        return true;
    }
    return as_boolean();
}

bool Expr::is_nil() const
{
    return get_type() == PAIR && as_pair()->empty();
}


shared_ptr<Context> Context::eval_apply_context(const shared_ptr<Expr>& proc, shared_ptr<Context> current_parent, const vector<Param>& params, shared_ptr<Pair>&& args)
{
    auto context = new_context(std::move(current_parent));
    shared_ptr<Pair> rest = std::move(args);
    size_t index = 0;
    while (index != params.size() && !rest->empty())
    {
        const auto& param = params[index];
        const auto& name = param.get_name();
        if (param.is_vararg())
        {
            context->add(name, Expr::make_pair(std::move(rest)));
            index = params.size();
            break;
        }
        const auto arg = this->eval(rest->car());
        rest = rest->cdr()->as_pair();
        context->add(name, arg);
        index++;
    }
    if (index != params.size())
    {
        throw GlomError("Incorrect number of arguments provided for " + proc->to_string());
    }

    return std::move(context);
}
shared_ptr<Expr> Context::eval(const shared_ptr<Pair>& exprs)
{
    shared_ptr<Expr> current = nullptr;
    for (const auto expr : *exprs)
    {
        if (!expr) break;
        current = eval(expr);
    }
    return current;
}


shared_ptr<Expr> Context::eval(const vector<shared_ptr<Expr>>& exprs)
{
    shared_ptr<Expr> current = nullptr;
    for (auto& expr : exprs)
    {
        current = eval(expr);
    }
    return current;
}
