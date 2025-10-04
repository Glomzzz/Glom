//
// Created by glom on 9/26/25.
//

#include <cmath>

#include "context.h"
#include "expr.h"
#include "error.h"

using std::make_shared;

Primitive::Primitive(string name, PrimitiveProc proc) : name(std::move(name)), proc(std::move(proc)){}

shared_ptr<Expr> Primitive::operator()(const shared_ptr<Context>& context, shared_ptr<Pair>&& args) const
{
    return proc(context, std::move(args));
}

const string& Primitive::get_name() const
{
    return name;
}

shared_ptr<Expr> make_primitive(const string& name, PrimitiveProc proc)
{
    return Expr::make_primitive(make_shared<Primitive>(name, std::move(proc)));
}

void Context::add_primitive(const string& name, PrimitiveProc proc)
{
    const auto name_view = SymbolPool::instance().intern(name);
    add(name_view, make_primitive(name, std::move(proc)));
}

void add_number_operations(Context& builder)
{
    builder.add_primitive("+", primitives::add);
    builder.add_primitive("*", primitives::mul);
    builder.add_primitive("-", primitives::sub);
    builder.add_primitive("/", primitives::div);
    builder.add_primitive("quotient", primitives::quotient);
    builder.add_primitive("modulo", primitives::modulo);
    builder.add_primitive("remainder", primitives::remainder);
    builder.add_primitive("expt", primitives::exponential);
}

void add_number_utils(Context& builder)
{
    builder.add_primitive("zero?", primitives::is_zero);
    builder.add_primitive("positive?", primitives::is_positive);
    builder.add_primitive("negative?", primitives::is_negative);
    builder.add_primitive("even?", primitives::is_even);
    builder.add_primitive("odd?", primitives::is_odd);
    builder.add_primitive("max", primitives::max);
    builder.add_primitive("min", primitives::min);
    builder.add_primitive("abs", primitives::abs);
    builder.add_primitive("gcd", primitives::gcd);
    builder.add_primitive("lcm", primitives::lcm);
    builder.add_primitive("floor", primitives::floor);
    builder.add_primitive("ceiling", primitives::ceiling);
    builder.add_primitive("truncate", primitives::truncate);
    builder.add_primitive("round", primitives::round);
}

void add_math_functions(Context& builder)
{
    builder.add_primitive("sqrt", primitives::square_root);
    builder.add_primitive("isqrt", primitives::square_root_integer);
    builder.add_primitive("log", primitives::logarithm);
    builder.add_primitive("sin", primitives::sine);
    builder.add_primitive("cos", primitives::cosine);
    builder.add_primitive("tan", primitives::tangent);
    builder.add_primitive("asin", primitives::arcsine);
    builder.add_primitive("acos", primitives::arccosine);
    builder.add_primitive("atan", primitives::arctangent);
    builder.add_primitive("exp", primitives::exponentiation);
}

void add_type_utils(Context& builder)
{
    builder.add_primitive("pair?", primitives::is_pair);
    builder.add_primitive("number?", primitives::is_number);
    builder.add_primitive("boolean?", primitives::is_boolean);
    builder.add_primitive("symbol?", primitives::is_symbol);
    builder.add_primitive("string?", primitives::is_string);
    builder.add_primitive("exact?", primitives::is_exact);
    builder.add_primitive("inexact?", primitives::is_inexact);
    builder.add_primitive("exact->inexact", primitives::exact_to_inexact);
    builder.add_primitive("inexact->exact", primitives::inexact_to_exact);
    builder.add_primitive("number->string", primitives::number_to_string);
    builder.add_primitive("string->number", primitives::string_to_number);
    builder.add_primitive("symbol->string", primitives::symbol_to_string);
    builder.add_primitive("string->symbol", primitives::string_to_symbol);
}

void add_quote_operation(Context& builder)
{
    builder.add_primitive("quote", primitives::quote);
}

void add_lambda(Context& builder)
{
    builder.add_primitive("lambda", primitives::lambda);
}

void add_new_bindings(Context& builder)
{
    builder.add_primitive("define", primitives::define);
    builder.add_primitive("let", primitives::let);
    builder.add_primitive("let*", primitives::let_star);
}

void add_number_comparators(Context& builder)
{
    builder.add_primitive("=", primitives::eq);

    builder.add_primitive("<", primitives::lt);
    builder.add_primitive(">", primitives::gt);
    builder.add_primitive("<=", primitives::le);

    builder.add_primitive(">=", primitives::ge);
}

void add_logic_operations(Context& builder)
{
    builder.add_primitive("and", primitives::logical_and);
    builder.add_primitive("or", primitives::logical_or);
    builder.add_primitive("not", primitives::logical_not);
}


void add_ptr_comparator(Context& builder)
{
    builder.add_primitive("eq?", primitives::eq_ptr);
    builder.add_primitive("eqv?", primitives::eq_val);
    builder.add_primitive("equal?", primitives::eq_struct);
    builder.add_primitive("string=?", primitives::eq_string);
    builder.add_primitive("string-ci=?", primitives::eq_string_ignore_case);
}

void add_condition(Context& builder)
{
    builder.add_primitive("if", primitives::cond_if);
    builder.add_primitive("cond", primitives::cond);
}

void add_io_operations(Context& builder)
{
    builder.add_primitive("display", primitives::display);
    builder.add_primitive("newline", primitives::newline);
    builder.add_primitive("read", primitives::read);
}
void add_list_operations(Context& builder)
{
    builder.add_primitive("cons", primitives::cons);
    builder.add_primitive("car", primitives::car);
    builder.add_primitive("caar", primitives::caar);
    builder.add_primitive("caaar", primitives::caaar);
    builder.add_primitive("caaaar", primitives::caaaar);
    builder.add_primitive("caaadr", primitives::caaadr);
    builder.add_primitive("caadr", primitives::caadr);
    builder.add_primitive("caadar", primitives::caadar);
    builder.add_primitive("caaddr", primitives::caaddr);
    builder.add_primitive("cadr", primitives::cadr);
    builder.add_primitive("cadar", primitives::cadar);
    builder.add_primitive("cadaar", primitives::cadaar);
    builder.add_primitive("cadadr", primitives::cadadr);
    builder.add_primitive("caddr", primitives::caddr);
    builder.add_primitive("caddar", primitives::caddar);
    builder.add_primitive("cadddr", primitives::cadddr);
    builder.add_primitive("cdr", primitives::cdr);
    builder.add_primitive("cdar", primitives::cdar);
    builder.add_primitive("cdaar", primitives::cdaar);
    builder.add_primitive("cdaaar", primitives::cdaaar);
    builder.add_primitive("cdaadr", primitives::cdaadr);
    builder.add_primitive("cdadr", primitives::cdadr);
    builder.add_primitive("cdadar", primitives::cdadar);
    builder.add_primitive("cdaddr", primitives::cdaddr);
    builder.add_primitive("cddr", primitives::cddr);
    builder.add_primitive("cddar", primitives::cddar);
    builder.add_primitive("cddaar", primitives::cddaar);
    builder.add_primitive("cddadr", primitives::cddadr);
    builder.add_primitive("cdddr", primitives::cdddr);
    builder.add_primitive("cdddar", primitives::cdddar);
    builder.add_primitive("cddddr", primitives::cddddr);
    builder.add_primitive("list", primitives::list);
    builder.add_primitive("null?", primitives::is_null);
    builder.add_primitive("append", primitives::append);
    builder.add_primitive("length", primitives::length);
}

void add_eval_control(Context& builder)
{
    builder.add_primitive("apply", primitives::apply);
    builder.add_primitive("call/cc", primitives::callcc);
    builder.add_primitive("error", primitives::error);
}



shared_ptr<Context> make_root_context()
{
    const shared_ptr<Context> context = Context::new_context();
    add_ptr_comparator(*context);
    add_number_operations(*context);
    add_number_comparators(*context);
    add_number_utils(*context);
    add_math_functions(*context);
    add_type_utils(*context);
    add_logic_operations(*context);
    add_quote_operation(*context);
    add_lambda(*context);
    add_new_bindings(*context);
    add_condition(*context);
    add_io_operations(*context);
    add_list_operations(*context);
    add_eval_control(*context);
    return context;
}
