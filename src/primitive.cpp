//
// Created by glom on 9/26/25.
//

#include <cmath>

#include "context.h"
#include "expr.h"

using std::make_shared;

Primitive::Primitive(string name, PrimitiveProc proc) : name(std::move(name)), proc(std::move(proc))
{
}

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
    add(name, make_primitive(name, std::move(proc)));
}

shared_ptr<Context> make_root_context()
{
    auto ctx = Context::new_context();
    return ctx;
}

