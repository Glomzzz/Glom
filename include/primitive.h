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
    bool generic_num_lt(shared_ptr<Expr>& a, shared_ptr<Expr>& b);
    bool generic_num_gt(shared_ptr<Expr>& a, shared_ptr<Expr>& b);
    void coerce_number(shared_ptr<Expr>& a, shared_ptr<Expr>& b);
    void expect_1_arg(const string& proc, const shared_ptr<Pair>& args, shared_ptr<Expr>& a);
    void expect_2_args(const string& proc, const shared_ptr<Pair>& args, shared_ptr<Expr>& a, shared_ptr<Expr>& b);
    void expect_2_or_3_args(const string& proc, const shared_ptr<Pair>& args, shared_ptr<Expr>& a, shared_ptr<Expr>& b, shared_ptr<Expr>& c);
    void take_car(const string& proc, const shared_ptr<Expr>& list, const shared_ptr<Expr>& expr, shared_ptr<Expr>& car);
    void take_cdr(const string& proc, const shared_ptr<Expr>& list, const shared_ptr<Expr>& expr, shared_ptr<Expr>& cdr);
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
    shared_ptr<Expr> quotient(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> remainder(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> modulo(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    // Math functions
    shared_ptr<Expr> exponentiation(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> logarithm(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> sine(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> cosine(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> tangent(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> arcsine(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> arccosine(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> arctangent(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> square_root(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);;
    shared_ptr<Expr> square_root_integer(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> exponential(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    // Number comparisons
    shared_ptr<Expr> eq(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> lt(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> gt(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> le(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> ge(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    // Number utils
    shared_ptr<Expr> is_zero(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> is_positive(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> is_negative(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> is_even(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> is_odd(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> max(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> min(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> abs(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> gcd(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> lcm(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> floor(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> ceiling(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> truncate(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> round(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
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
    shared_ptr<Expr> is_boolean(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> is_symbol(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> is_string(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> is_exact(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> is_inexact(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> exact_to_inexact(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> inexact_to_exact(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> number_to_string(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> string_to_number(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> symbol_to_string(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> string_to_symbol(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    // IO
    shared_ptr<Expr> display(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> read(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> newline(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    // Pair
    shared_ptr<Expr> is_null(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> cons(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> car(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> caar(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> caaar(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> caaaar(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> caaadr(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> caadr(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> caadar(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> caaddr(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> cadr(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> cadar(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> cadaar(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> cadadr(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> caddr(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> caddar(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> cadddr(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> cdr(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> cdar(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> cdaar(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> cdaaar(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> cdaadr(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> cdadr(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> cdadar(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> cdaddr(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> cddr(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> cddar(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> cddaar(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> cddadr(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> cdddr(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> cdddar(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> cddddr(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> list(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
    shared_ptr<Expr> append(const shared_ptr<Context>& context, shared_ptr<Pair>&& args);
}
#endif //GLOM_PRIMITIVE_H