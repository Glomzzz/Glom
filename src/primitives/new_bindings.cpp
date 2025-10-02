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

        if (name_params_expr->is_symbol())
        {
            name = name_params_expr->as_symbol();
            auto value_expr = std::move(current->car());
            current = current->cdr()->as_pair();
            value = eval(context, std::move(value_expr));;
            if (!current->empty())
            {
                throw GlomError("Invalid body define");
            }
        }
        else if (name_params_expr->is_pair())
        {
            const auto terms = name_params_expr->as_pair();
            if (terms->empty())
            {
                throw GlomError("Invalid define, no name");
            }
            const auto first = std::move(terms->car());
            auto rest = terms->cdr()->as_pair();
            if (!first->is_symbol())
            {
                throw GlomError("Invalid define, name must be symbol");
            }
            name = first->as_symbol();

            shared_ptr<Expr> lambda_params_expr = nullptr;
            if (const auto second = rest->car(); second && second->is_symbol() && second->as_symbol() == ".")
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
    shared_ptr<Pair> current = std::move(args);
    const auto first = current->car();
    current = current->cdr()->as_pair();
    shared_ptr<Expr> name = nullptr;
    shared_ptr<Expr> bindings_expr = nullptr;
    if (first->is_symbol())
    {
        name = first;
        bindings_expr = current->car();
        current = current->cdr()->as_pair();
    }
    else
    {
        bindings_expr = first;
    }
    if (!bindings_expr->is_pair())
    {
        throw GlomError("Invalid bindings in let");
    }
    const auto bindings_list = bindings_expr->as_pair();
    auto lambda_params = vector<Param>();
    shared_ptr<Pair> lambda_args = nullptr;
    for (shared_ptr<Pair> lambda_args_tail = nullptr; const auto& binding_expr : *bindings_list)
    {
        if (!binding_expr) break;
        if (!binding_expr->is_pair())
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
        if (!name_expr->is_symbol())
        {
            throw GlomError("Invalid binding name in let, must be symbol");
        }
        if (!rest->cdr()->is_nil())
        {
            throw GlomError("Invalid binding in let, must have exactly one value");
        }
        const auto binding_name = name_expr->as_symbol();
        auto lambda_param = Param(binding_name, false);
        shared_ptr<Expr> lambda_arg = eval(context, std::move(rest->car()));
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
    auto body = current;
    shared_ptr<Context> lambda_context = Context::new_context(context);
    auto lambda = Expr::make_lambda(make_shared<Lambda>(std::move(lambda_params), std::move(body), lambda_context));
    if (name)
    {
        lambda_context->add(name->as_symbol(), lambda);
    }
    const auto apply = Pair::cons(std::move(lambda), Expr::make_pair(lambda_args));
    return eval(context, Expr::make_pair(apply));
}