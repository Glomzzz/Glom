#include "context.h"
#include "expr.h"
#include "primitive.h"
#include "error.h"
#include "eval.h"

using std::string;

// set!  — (set! <symbol> <expr>)
shared_ptr<Expr> primitives::set(const shared_ptr<Context>& context, shared_ptr<Pair>&& args) {
    shared_ptr<Expr> nameExpr, valueExpr;
    primitives_utils::expect_2_args("set!", args, nameExpr, valueExpr);

    if (!nameExpr->is_symbol()) {
        throw GlomError("set!: first argument must be a symbol");
    }

    // Only evaluate the value (the variable name is *not* evaluated).
    const auto value = eval(context, valueExpr);

    if (const auto name = nameExpr->as_symbol(); !context->assign(name, value)) {
        throw GlomError("set!: unbound variable " + view_to_string(name));
    }

    // R5RS: return unspecified; use your sentinel
    return Expr::NOTHING;
}

// set-car! — (set-car! <pair> <value>)
shared_ptr<Expr> primitives::set_car(const shared_ptr<Context>& context, shared_ptr<Pair>&& args) {
    shared_ptr<Expr> pairExpr, valExpr;
    primitives_utils::expect_2_args("set-car!", args, pairExpr, valExpr);

    // Both operands are evaluated (this is a procedure, not a special form)
    pairExpr = eval(context, pairExpr);
    valExpr  = eval(context, valExpr);

    if (!pairExpr->is_pair()) {
        throw GlomError("set-car!: first argument is not a pair");
    }
    const auto pair = pairExpr->as_pair();
    if (!pair || pair->empty()) {
        throw GlomError("set-car!: cannot mutate the empty list");
    }

    pair->set_car(valExpr);
    return Expr::NOTHING;
}

// set-cdr! — (set-cdr! <pair> <value>)
shared_ptr<Expr> primitives::set_cdr(const shared_ptr<Context>& context, shared_ptr<Pair>&& args) {
    shared_ptr<Expr> pairExpr, valExpr;
    primitives_utils::expect_2_args("set-cdr!", args, pairExpr, valExpr);

    pairExpr = eval(context, pairExpr);
    valExpr  = eval(context, valExpr);

    if (!pairExpr->is_pair()) {
        throw GlomError("set-cdr!: first argument is not a pair");
    }
    const auto pair = pairExpr->as_pair();
    if (!pair || pair->empty()) {
        throw GlomError("set-cdr!: cannot mutate the empty list");
    }

    // CDR can be any list/pair/NIL
    pair->set_cdr(valExpr);
    return Expr::NOTHING;
}
