//
// Created by glom on 9/26/25.
//

#include "eval.h"

#include <utility>

#include "context.h"
#include "expr.h"
#include "error.h"


shared_ptr<Context> eval_apply_context(const shared_ptr<Context>& ctx, const shared_ptr<Expr>& proc, const shared_ptr<Context>& current_parent, const vector<Param>& params, shared_ptr<Pair>&& args)
{
    auto context = Context::new_context(current_parent);
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
        const auto arg = eval(ctx, rest->car());
        rest = rest->cdr()->as_pair();
        context->add(name, arg);
        index++;
    }
    if (index != params.size())
    {
        throw GlomError("Incorrect number of arguments provided for " + proc->to_string());
    }

    return context;
}

GlomCont::GlomCont(unique_ptr<Continuation> cont, shared_ptr<Expr> value) : cont(std::move(cont)), value(std::move(value)) {}

shared_ptr<Expr> eval(const shared_ptr<Context>& ctx, shared_ptr<Expr> expr)
{
    auto body = Pair::single(std::move(expr));
    return eval(ctx, std::move(body));
}

shared_ptr<Expr> eval(const shared_ptr<Context>& ctx, shared_ptr<Pair> rest)
{
    auto current_ctx = ctx;
    shared_ptr<Expr> expr = nullptr;
    rest = std::move(rest);
    if (rest->empty())
    {
        throw GlomError("Cannot evaluate null expression");
    }
    bool tail = false;
    while (true)
    {
        expr = rest->car();
        rest = rest->cdr()->as_pair();
        if (!tail)
            tail = rest->empty();
        if (expr->is_symbol())
        {
            const auto& name = expr->as_symbol();
            auto var = current_ctx->get(name);
            if (!var)
            {
                throw GlomError("Undefined variable: " + view_to_string(name));
            }
            if (tail)
            {
                return var;
            }
            continue;
        }
        if (!expr->is_pair())
        {
            if (tail)
            {
                return std::move(expr);
            }
            continue;
        }
        const auto pair = expr->as_pair();
        const auto proc = eval(current_ctx,std::move(pair->car()));
        auto args = pair->cdr()->as_pair();
        if (proc->is_primitive())
        {
            const auto primitive = proc->as_primitive();
            auto result = (*primitive)(current_ctx,std::move(args));
            if (result->is_cont())
            {
                auto& [cont_ctx, cont_exprs, callcc] = result->as_cont();
                if (callcc)
                {
                    auto current_cont = make_continuation(current_ctx, std::move(rest));
                    cont_ctx->add(*callcc, std::move(current_cont));
                    try
                    {
                       auto eval_cont = eval(cont_ctx, std::move(cont_exprs));
                        if (tail)
                        {
                            return eval_cont;
                        }
                        continue;
                    }
                    catch (GlomCont& glom_cont)
                    {
                        if (tail)
                        {
                            return glom_cont.value;
                        }
                        const auto cont = std::move(glom_cont.cont);
                        current_ctx = cont->context;
                        rest = cont->exprs;
                        continue;
                    }
                }
                if (tail)
                {
                    current_ctx = cont_ctx;
                    rest = cont_exprs;
                    continue;
                }
                eval(cont_ctx, std::move(cont_exprs));
            }
            if (tail)
            {
                return result;
            }
        }
        if (proc->is_lambda())
        {
            const auto lambda = proc->as_lambda();
            const auto& params = lambda->get_params();
            const auto& body = lambda->get_body();
            shared_ptr<Context> apply_context = nullptr;
            if (params.empty())
            {
                apply_context = lambda->get_context();
            }
            else
            {
                apply_context = eval_apply_context(current_ctx, proc, lambda->get_context(), params, std::move(args));
            }
            if (tail)
            {
                current_ctx = apply_context;
                rest = body;
                continue;
            }
            eval(apply_context, body);
        }
        if (proc->is_cont())
        {
            if (args->empty() || args->cdr()->is_nil() && args->car()->is_nil())
            {
                throw GlomError("Continuation requires one argument: " + expr->to_string());
            }
            auto result = eval(current_ctx, args->car());
            auto& [cont_ctx, cont_exprs, _] = proc->as_cont();
            throw GlomCont(std::make_unique<Continuation>(cont_ctx, std::move(cont_exprs)), std::move(result));
        }
        throw GlomError( proc->to_string() + " is not a procedure: " + expr->to_string());
    }
}