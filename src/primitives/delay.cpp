#include "context.h"
#include "expr.h"
#include "primitive.h"
#include "error.h"
#include "eval.h"

namespace {

// Build a zero-arg lambda that closes over `context` and has `body` as its body list.
 shared_ptr<Expr> make_thunk(const shared_ptr<Context>& context, const shared_ptr<Pair>& body) {
    std::vector<Param> params; // zero-arg
    const auto lam = std::make_shared<Lambda>(std::move(params), body, context);
    return Expr::make_lambda(lam);
}

// Recognize our promise layout and unpack it.
// Returns (forcedFlagPair, cellPair) where:
// - forcedFlagPair is the pair whose CAR is the boolean forced flag
// - cellPair       is the pair whose CAR is the thunk-or-value
// Throws GlomError if not a promise.
 std::pair<shared_ptr<Pair>, shared_ptr<Pair>>
as_promise_pairs(const shared_ptr<Expr>& e) {
    if (!e || !e->is_pair()) {
        throw GlomError("force: argument is not a promise");
    }
    const auto p = e->as_pair();
    if (!p || p->empty()) {
        throw GlomError("force: argument is not a promise");
    }
    if (const auto tag = p->car(); !tag || !tag->is_symbol() || view_to_string(tag->as_symbol()) != "##promise") {
        throw GlomError("force: argument is not a promise");
    }
    const auto payloadExpr = p->cdr();
    if (!payloadExpr || !payloadExpr->is_pair()) {
        throw GlomError("force: malformed promise");
    }
    auto forcedPair = payloadExpr->as_pair(); // (forced . rest)
    if (forcedPair->empty()) {
        throw GlomError("force: malformed promise");
    }
    const auto restExpr = forcedPair->cdr();
    if (!restExpr || !restExpr->is_pair()) {
        throw GlomError("force: malformed promise");
    }
    auto cellPair = restExpr->as_pair(); // (cell . ())
    return { forcedPair, cellPair };
}

} // namespace

// delay — special form: (delay <expr>)
// Return a promise that, when forced, evaluates <expr> in the current environment exactly once.
shared_ptr<Expr> primitives::delay(const shared_ptr<Context>& context, shared_ptr<Pair>&& args) {
    // Expect exactly one (unevaluated) expression in args.
    shared_ptr<Expr> bodyExpr;
    {
        // Manual 1-arg check (we must NOT eval the argument here).
        if (!args || args->empty()) {
            throw GlomError("delay: exactly one expression required");
        }
        bodyExpr = args->car();
        if (const auto rest = args->cdr(); rest && rest->is_pair() && !rest->as_pair()->empty()) {
            throw GlomError("delay: exactly one expression required");
        }
    }

    // Create the thunk: (lambda () <expr>)
    // The Lambda body is a list of expressions; here just a single element list.
    const auto bodyList = Pair::single(bodyExpr);
    const auto thunk = make_thunk(context, bodyList);

    // Build tagged promise structure:
    // (##promise #f thunk)
    const auto tag = Expr::make_symbol(string("##promise"));
    const auto forcedFlag = Expr::make_boolean(false);

    const auto cellPair = Pair::single(thunk);                          // (thunk)
    const auto payload  = Pair::cons(forcedFlag, Expr::make_pair(cellPair)); // (forced . (thunk))
    const auto promise  = Pair::cons(tag,       Expr::make_pair(payload));   // (##promise . (forced . (thunk)))

    return Expr::make_pair(promise);
}

// force — procedure: (force <promise>)
// Evaluate the thunk the first time, memoize the value, and return it thereafter.
shared_ptr<Expr> primitives::force(const shared_ptr<Context>& context, shared_ptr<Pair>&& args) {
    shared_ptr<Expr> promiseExpr;
    primitives_utils::expect_1_arg("force", args, promiseExpr);
    promiseExpr = eval(context, promiseExpr); // force is a procedure: its argument is evaluated

    auto [forcedPair, cellPair] = as_promise_pairs(promiseExpr);

    if (const auto forcedFlagExpr = forcedPair->car(); forcedFlagExpr && forcedFlagExpr->is_boolean() && forcedFlagExpr->as_boolean()) {
        // Already forced: return memoized value
        return cellPair->car();
    }

    // Not yet forced: evaluate the thunk
    const auto thunkOrVal = cellPair->car();
    if (!thunkOrVal || !thunkOrVal->is_lambda()) {
        throw GlomError("force: malformed promise (no thunk)");
    }

    // Build application: (thunk)
    const auto app = Expr::make_pair(Pair::cons(thunkOrVal, Expr::NIL));
    auto value = eval(context, app);

    // Memoize: set forced? := #t, replace cell with computed value
    forcedPair->set_car(Expr::make_boolean(true));
    cellPair->set_car(value);

    return value;
}
