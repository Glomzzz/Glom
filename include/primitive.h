//
// Created by glom on 9/26/25.
//

#ifndef GLOM_PRIMITIVE_H
#define GLOM_PRIMITIVE_H

#include <functional>
#include <vector>
#include <string>
#include <memory>
#include "eval.h"

struct Continuation;
class Pair;
class Context;
class Expr;

using std::string;
using std::vector;
using std::unique_ptr;
using std::shared_ptr;
using PrimitiveProc = std::function<shared_ptr<Expr>(shared_ptr<Context>, shared_ptr<Pair>&&)>;

class Primitive {
    string name;
    PrimitiveProc proc;
public:
    explicit Primitive(string name, PrimitiveProc proc);

    shared_ptr<Expr> operator()(const shared_ptr<Context>& context, shared_ptr<Pair>&& args) const;

    [[nodiscard]] const string& get_name() const;
};


namespace primitives_utils
{
    bool generic_num_eq(shared_ptr<Expr>& a, shared_ptr<Expr>& b);
    void coerce_number(shared_ptr<Expr>& a, shared_ptr<Expr>& b);
    void expect_1_arg(const string& proc, const shared_ptr<Pair>& args, shared_ptr<Expr>& a);
    void expect_2_args(const string& name, const shared_ptr<Pair>& args, shared_ptr<Expr>& a, shared_ptr<Expr>& b);
    void expect_2_or_3_args(const string& name, const shared_ptr<Pair>& args, shared_ptr<Expr>& a, shared_ptr<Expr>& b, shared_ptr<Expr>& c);
}

namespace primitives
{
    // Eval Control
    shared_ptr<Expr> apply(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> callcc(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);

    // Quote
    shared_ptr<Expr> quote(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    // Number operations
    shared_ptr<Expr> add(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> sub(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> mul(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> div(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> remainder(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> modulo(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> power(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    // Number comparisons
    shared_ptr<Expr> eq(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> lt(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> gt(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> le(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> ge(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    // Logic operations
    shared_ptr<Expr> logical_and(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> logical_or(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> logical_not(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    // Lambda
    shared_ptr<Expr> lambda(shared_ptr<Context> context, const shared_ptr<Pair>&& args);
    // New bindings
    shared_ptr<Expr> define(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> let(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    // Condition
    shared_ptr<Expr> cond_if(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> cond(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    // Equal
    shared_ptr<Expr> eq_ptr(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> eq_val(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> eq_struct(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    // Type
    shared_ptr<Expr> is_pair(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> is_number(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    // IO
    shared_ptr<Expr> display(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> read(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> newline(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    // List
    shared_ptr<Expr> is_null(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> cons(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> car(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> cdr(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> list(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
}
#endif //GLOM_PRIMITIVE_H