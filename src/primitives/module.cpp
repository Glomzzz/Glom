#include "primitive.h"
#include "expr.h"
#include "context.h"
#include "module.h"

#include "error.h"

using std::string;
using std::shared_ptr;

// Helpers to read unevaluated symbols/strings -----------------
static bool is_symbol_named(const shared_ptr<Expr>& e, const char* s) {
    return e && e->is_symbol() && view_to_string(e->as_symbol()) == s;
}

// PROVIDE -----------------------------------------------------

// (provide (all-defined-out)) | (provide a b c)
shared_ptr<Expr> primitives::provide(const shared_ptr<Context>& context, shared_ptr<Pair>&& args) {
    if (!args || args->empty())
        throw GlomError("provide: at least one export spec required");

    // Case 1: (provide (all-defined-out))
    if (const auto first = args->car(); first && first->is_pair()) {
        if (const auto p = first->as_pair(); !p->empty() && is_symbol_named(p->car(), "all-defined-out")) {
            context->provide_all();
            return Expr::NOTHING;
        }
    }

    // Case 2: (provide a b (…)) — we only accept symbols as args here.
    for (const auto& spec : *args) {
        if (!spec) break;
        if (!spec->is_symbol()) {
            throw GlomError("provide: expected symbols or (all-defined-out)");
        }
        auto name_sv = spec->as_symbol();
        context->provide({name_sv});
    }

    return Expr::NOTHING;
}

// REQUIRE -----------------------------------------------------

// Shapes:
//   (require "foo.gl")
//   (require (only-in "foo.gl" a b (c c1)))
shared_ptr<Expr> primitives::require(const shared_ptr<Context>& context, shared_ptr<Pair>&& args) {
    if (!args || args->empty())
        throw GlomError("require: expected a module spec");

    const auto spec = args->car();

    // (require "foo.gl")
    if (spec->is_string()) {
        const auto path = spec->as_string();
        const auto mod = Module::load_from_file(path);
        const auto exports = mod.get_exports();
        if (!exports) {
            // No provide was used; nothing to import.
            return Expr::NOTHING;
        }
        context->import_all(exports);
        return Expr::NOTHING;
    }

    // (require (only-in "foo.gl" a b (c c1)))
    if (spec && spec->is_pair()) {
        const auto p = spec->as_pair();
        if (p->empty() || !is_symbol_named(p->car(), "only-in"))
            throw GlomError("require: unknown import spec; expected (only-in ...) or string path");

        // Parse: path then a sequence of selectors (symbols or (from to))
        const auto rest = p->cdr();
        if (!rest || !rest->is_pair() || rest->as_pair()->empty())
            throw GlomError("require: (only-in <path> <names...>)");

        auto first = rest->as_pair()->car();
        if (!first->is_string())
            throw GlomError("require: first arg to only-in must be a string path");

        auto path = first->to_string();
        auto mod = Module::load_from_file(path);
        auto exports = mod.get_exports();
        if (!exports) return Expr::NOTHING;
        // Build mapping list
        const auto names_expr = rest->as_pair()->cdr(); // list of selectors
        for (auto it = names_expr; it && it->is_pair() && !it->as_pair()->empty(); it = it->as_pair()->cdr()) {
            const auto sel = it->as_pair()->car();

            // symbol: import same name
            if (sel->is_symbol()) {
                auto from = sel->as_symbol();
                if (!exports->has(from))
                    throw GlomError("require/only-in: not exported: " + view_to_string(from));
                context->add(from, exports->get(from));
                continue;
            }

            // (from to) renaming
            if (sel->is_pair()) {
                const auto pair = sel->as_pair();
                if (pair->empty()) throw GlomError("require/only-in: empty rename");
                const auto fromE = pair->car();
                const auto rest2 = pair->cdr();
                if (!fromE || !fromE->is_symbol() || !rest2 || !rest2->is_pair() || rest2->as_pair()->empty())
                    throw GlomError("require/only-in: expected (from to)");
                const auto toE = rest2->as_pair()->car();
                if (!toE || !toE->is_symbol())
                    throw GlomError("require/only-in: expected (from to)");

                auto from = fromE->as_symbol();
                auto   to = toE->as_symbol();
                if (!exports->has(from))
                    throw GlomError("require/only-in: not exported: " + view_to_string(from));
                context->add(to, exports->get(from));
                continue;
            }

            throw GlomError("require/only-in: bad selector");
        }

        return Expr::NOTHING;
    }

    throw GlomError("require: bad module spec");
}
