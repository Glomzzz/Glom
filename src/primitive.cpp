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
    builder.add_primitive("modulo", primitives::modulo);
    builder.add_primitive("remainder", primitives::remainder);
    builder.add_primitive("expt", primitives::power);
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
    builder.add_primitive("cdr", primitives::cdr);
    builder.add_primitive("list", primitives::list);
    builder.add_primitive("null?", primitives::is_null);
}

void add_eval_control(Context& builder)
{
    builder.add_primitive("apply", primitives::apply);
    builder.add_primitive("call/cc", primitives::callcc);
}



shared_ptr<Context> make_root_context()
{
    const shared_ptr<Context> context = Context::new_context();
    add_ptr_comparator(*context);
    add_number_operations(*context);
    add_number_comparators(*context);
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
