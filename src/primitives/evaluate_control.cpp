//
// Created by glom on 9/29/25.
//

#include "context.h"
#include "error.h"
#include "expr.h"
#include "primitive.h"

shared_ptr<Expr> primitives::apply(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> proc, proc_args;
    primitives_utils::expect_2_args("apply", args, proc, proc_args);
    proc = eval(context, proc);
    proc_args = eval(context, proc_args);
    const auto apply = Expr::make_pair(Pair::cons(proc,proc_args));
    return eval(context, apply);
}

shared_ptr<Expr> primitives::callcc(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> proc;
    primitives_utils::expect_1_arg("call/cc", args, proc);
    proc = eval(context, proc);
    if (!proc->is_lambda())
        throw GlomError("call/cc: argument is not a procedure");

    const auto lambda = proc->as_lambda();
    const auto params = lambda->get_params();
    if (params.size() != 1)
        throw GlomError("call/cc: procedure must take exactly one argument");
    const auto param = params[0];
    if (param.is_vararg())
        throw GlomError("call/cc: procedure argument cannot be vararg");
    const auto param_name = param.get_name();
    const auto ctx = lambda->get_context();
    auto body = lambda->get_body();
    auto cont = make_callcc(ctx, std::move(body), param_name);
    return cont;
}

//error
shared_ptr<Expr> primitives::error(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    if (args->empty())
        throw GlomError("error: at least one argument required");
    std::string message;
    for (const auto& arg : *args)
    {
        if (!arg) break;
        if (message.empty())
            message = arg->to_string();
        else
            message += " " + arg->to_string();
    }
    throw GlomError("Error: " + message);
}