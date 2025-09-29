//
// Created by glom on 9/27/25.
//

#include "error.h"
#include "context.h"
#include "expr.h"
#include "primitive.h"

bool eq_ptr_impl(const shared_ptr<Expr>& a, const shared_ptr<Expr>& b) {
    if (a->get_type() != b->get_type()) return false;
    switch (a->get_type())
    {
    case NUMBER:
        return a->as_number() == b->as_number();
    case SYMBOL:
        return a->as_symbol().data() == b->as_symbol().data();
    default:
        return a.get() == b.get();
    }
}

shared_ptr<Expr> primitives::eq_ptr(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> a, b = nullptr;
    primitives_utils::expect_2_args("eq?", args, a, b);
    a = context->eval(a);
    b = context->eval(b);
    return Expr::make_boolean(eq_ptr_impl(a,b));
}
shared_ptr<Expr> primitives::eq_val(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> a, b = nullptr;
    primitives_utils::expect_2_args("eqv?", args, a, b);
    a = context->eval(a);
    b = context->eval(b);
    if (eq_ptr_impl(a,b)) return Expr::TRUE;
    switch (a->get_type())
    {
    case NUMBER:
        return Expr::make_boolean(a->as_number() == b->as_number());
    case BOOLEAN:
        return Expr::make_boolean(a->as_boolean() == b->as_boolean());
    case STRING:
        return Expr::make_boolean(a->as_string() == b->as_string());
    case SYMBOL:
        return Expr::make_boolean(a->as_symbol().data() == b->as_symbol().data());
    default:
        return Expr::FALSE;
    }
}


bool equal_internal(const shared_ptr<Expr>& a, const shared_ptr<Expr>& b,
                    unordered_map<const Expr*, const Expr*>& visited) {

    switch (a->get_type()) {
    case NUMBER:
        return a->as_number() == b->as_number();
    case BOOLEAN:
        return a->as_boolean() == b->as_boolean();
    case STRING:
        return a->as_string() == b->as_string();
    case SYMBOL:
        return a->as_symbol().data() == b->as_symbol().data();
    case PAIR: {
            const auto pa = a->as_pair();
            const auto pb = b->as_pair();
            if (!pa || !pb) return pa == pb; // both null -> equal; one null -> not equal
            if (pa->empty() && pb->empty()) return true;
            if (pa->empty() || pb->empty()) return false;
            // cycle detection: if 'a' previously mapped it must map to 'b'
            if (const auto it = visited.find(a.get()); it != visited.end()) return it->second == b.get();
            visited[a.get()] = b.get();

            if (!equal_internal(pa->car(), pb->car(), visited)) return false;
            if (!equal_internal(pa->cdr(), pb->cdr(), visited)) return false;
            return true;
    }
    case LAMBDA:
    case PRIMITIVE:
        return a.get() == b.get();
    default:
        return false;
    }
}

shared_ptr<Expr> primitives::eq_struct(const shared_ptr<Context>& context, shared_ptr<Pair>&& args)
{
    shared_ptr<Expr> a, b = nullptr;
    primitives_utils::expect_2_args("equal?", args, a, b);
    a = context->eval(a);
    b = context->eval(b);
    if (eq_ptr_impl(a,b)) return Expr::TRUE;
    unordered_map<const Expr*, const Expr*> visited;
    visited.reserve(32);
    return Expr::make_boolean(equal_internal(a, b, visited));
}