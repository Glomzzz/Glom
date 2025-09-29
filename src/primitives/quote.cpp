//
// Created by glom on 9/27/25.
//

#include "error.h"
#include "expr.h"
#include "primitive.h"

shared_ptr<Expr> primitives::quote(const shared_ptr<Context>& _, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> arg = nullptr;
    primitives_utils::expect_1_arg("quote", args, arg);
    return arg;
}