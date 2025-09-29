//
// Created by glom on 9/29/25.
//

#include "context.h"
#include "expr.h"
#include "primitive.h"

shared_ptr<Expr> primitives::apply(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> proc, proc_args;
    primitives_utils::expect_2_args("apply", args, proc, proc_args);
    proc = context->eval(proc);
    proc_args = context->eval(proc_args);
    const auto apply = Expr::make_pair(Pair::cons(proc,proc_args));
    return context->eval(apply);
}