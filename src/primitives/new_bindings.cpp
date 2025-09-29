//
// Created by glom on 9/27/25.
//

#include "error.h"
#include "context.h"
#include "expr.h"
#include "primitive.h"


shared_ptr<Expr> primitives::define(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
    {
        auto current = args;

        const auto name_params_expr = std::move(current->car());
        current = current->cdr()->as_pair();
        if (current->empty())
        {
            throw GlomError("Invalid number of arguments define");
        }
        string_view name;
        shared_ptr<Expr> value;

        if (name_params_expr->get_type() == SYMBOL)
        {
            name = name_params_expr->as_symbol();
            auto value_expr = std::move(current->car());
            current = current->cdr()->as_pair();
            value = context->eval(std::move(value_expr));;
            if (!current->empty())
            {
                throw GlomError("Invalid body define");
            }
        }
        else if (name_params_expr->get_type() == PAIR)
        {
            const auto terms = name_params_expr->as_pair();
            if (terms->empty())
            {
                throw GlomError("Invalid define, no name");
            }
            const auto first = std::move(terms->car());
            auto rest = terms->cdr()->as_pair();
            if (first->get_type() != SYMBOL)
            {
                throw GlomError("Invalid define, name must be symbol");
            }
            name = first->as_symbol();

            shared_ptr<Expr> lambda_params_expr = nullptr;
            if (const auto second = rest->car(); second && second->get_type() == SYMBOL && second->as_symbol() == ".")
            {
                rest = rest->cdr()->as_pair();
                if (rest->empty())
                {
                    throw GlomError("Invalid define, no parameter after dot (.)");
                }
                lambda_params_expr = std::move(rest->car());
                rest = rest->cdr()->as_pair();
                if (!rest->empty())
                {
                    throw GlomError("Invalid define, more than one parameter after dot (.)");
                }
            }
            else
            {
                lambda_params_expr = terms->cdr();
            }
            shared_ptr<Pair> lambda_args = Pair::cons(std::move(lambda_params_expr), std::move(args->cdr()));
            // Reuse make_lambda to create the lambda expression
            value = lambda(context, std::move(lambda_args));
        }
        else
        {
            throw GlomError("Invalid parameters define: name must be symbol or list");
        }
        context->add(name, std::move(value));
        return Expr::NOTHING;
    }
shared_ptr<Expr> primitives::let(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty() || args->cdr()->is_nil())
    {
        throw GlomError("Invalid number of arguments let");
    }
    const auto bindings = args->car();
    if (bindings->get_type() != PAIR)
    {
        throw GlomError("Invalid bindings in let");
    }
    const auto bindings_list = bindings->as_pair();
    auto lambda_params = vector<Param>();
    shared_ptr<Pair> lambda_args = nullptr;
    for (shared_ptr<Pair> lambda_args_tail = nullptr; const auto& binding_expr : *bindings_list)
    {
        if (!binding_expr) break;
        if (binding_expr->get_type() != PAIR)
        {
            throw GlomError("Invalid binding in let");
        }
        const auto binding = binding_expr->as_pair();
        if (binding->empty() || binding->cdr()->is_nil())
        {
            throw GlomError("Invalid binding in let");
        }
        const auto name_expr = binding->car();
        const auto rest = binding->cdr()->as_pair();
        if (name_expr->get_type() != SYMBOL)
        {
            throw GlomError("Invalid binding name in let, must be symbol");
        }
        if (!rest->cdr()->is_nil())
        {
            throw GlomError("Invalid binding in let, must have exactly one value");
        }
        const auto name = name_expr->as_symbol();
        auto lambda_param = Param(name, false);
        shared_ptr<Expr> lambda_arg = context->eval(std::move(rest->car()));
        lambda_params.emplace_back(lambda_param);
        const auto new_tail = Pair::single(std::move(lambda_arg));
        if (!lambda_args)
        {
            lambda_args = new_tail;
            lambda_args_tail = lambda_args;
        }
        else
        {
            lambda_args_tail->set_cdr(Expr::make_pair(new_tail));
            lambda_args_tail = new_tail;
        }
    }
    auto body = args->cdr()->as_pair();
    auto lambda = make_shared<Lambda>(std::move(lambda_params), std::move(body), context);
    const auto apply = Pair::cons(Expr::make_lambda(std::move(lambda)), Expr::make_pair(lambda_args));
    return context->eval(Expr::make_pair(apply));
}