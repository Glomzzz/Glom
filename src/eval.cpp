//
// Created by glom on 9/26/25.
//
#include "eval.h"
#include "context.h"
#include "expr.h"

EvalError::EvalError(const std::string& msg) : std::runtime_error(msg) {}

template<typename T>
shared_ptr<Expr> Context::eval_impl(T&& expr) {
    if (!expr)
    {
        throw EvalError("Cannot evaluate null expression");
    }

    switch (expr->get_type())
    {
    case NUMBER:
    case STRING:
    case BOOLEAN:
    case LAMBDA:
    case PRIMITIVE:
        return std::forward<T>(expr);

    case SYMBOL:
        {
            const auto& name = expr->as_string();
            auto var = this->get(name);
            if (!var)
            {
                throw EvalError("Undefined variable: " + name);
            }
            return var;
        }

    case LIST:
        {
            auto list = getList(std::forward<T>(expr));
            if (list.empty())
            {
                throw EvalError("Cannot evaluate empty list");
            }

            auto proc = eval(std::move(list[0]));
            vector<shared_ptr<Expr>> args;
            args.reserve(list.size() - 1);
            for (size_t i = 1; i < list.size(); ++i) {
                args.push_back(std::move(list[i]));
            }

            return apply(std::move(proc), std::move(args));
        }
    default:
        throw EvalError("Unsupported expression type");
    }
}


template<typename T>
auto getList(T&& expr) {
    if constexpr (std::is_lvalue_reference_v<T>) {
        return expr->as_list(); //Left value: reference
    } else {
        return std::move(expr->as_list());  //Right value: move
    }
}

shared_ptr<Expr> Context::eval(const shared_ptr<Expr>& expr)
{
    return eval_impl(expr);
}


shared_ptr<Expr> Context::eval(shared_ptr<Expr>&& expr)
{
    return eval_impl(std::move(expr));
}


shared_ptr<Context> eval_apply_context(const shared_ptr<Context>& parent, const vector<Param>& params, vector<shared_ptr<Expr>>&& args)
{
    auto context_builder = ContextBuilder(parent);
    for (int i = 0; i < args.size(); i++)
    {
        auto& param = params[i];
        const auto& name = param.get_name();
        if (param.is_vararg())
        {
            auto rest_args = std::vector<shared_ptr<Expr>>();
            for (int j = i; j < args.size(); j++)
            {
                rest_args.push_back(std::move(args[j]));
            }
            const auto rest = std::make_shared<Expr>(Expr::make_list(std::move(rest_args)));
            context_builder.set(name, std::move(rest));
            break;
        }
        context_builder.set(name,std::move(args[i]));
    }
    return context_builder.build();
}

shared_ptr<Expr> Context::apply(shared_ptr<Expr>&& proc, vector<shared_ptr<Expr>>&& args)
{
    switch (proc->get_type())
    {
        case PRIMITIVE:
            {
                const auto primitive = proc->as_primitive();
                return (*primitive)(std::move(args));
            }
        case LAMBDA:
            {
                const auto lambda = proc->as_lambda();
                const auto& params = lambda->get_params();
                const auto& body = lambda->get_body();
                shared_ptr<Context> context = nullptr;
                if (params.empty())
                {
                    context = lambda->get_context();
                } else
                {
                    context = eval_apply_context(lambda->get_context(), params, std::move(args));
                }
                return context->eval(body);
            }
        default:
            throw EvalError("First element in list is not a function");
    }
}

shared_ptr<Expr> Context::eval(vector<shared_ptr<Expr>>&& exprs)
{
    shared_ptr<Expr> current = nullptr;
    for (auto& expr : exprs)
    {
        current = eval(std::move(expr));
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

