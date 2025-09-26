//
// Created by glom on 9/26/25.
//
#include "context.h"

#include <utility>


ContextBuilder::ContextBuilder(shared_ptr<Context> p) : parent(std::move(p)) {}

ContextBuilder& ContextBuilder::set(const string& name, shared_ptr<Expr> value) {
    bindings[name] = std::move(value);
    return *this;
}

ContextBuilder& ContextBuilder::setParent(shared_ptr<Context> p) {
    parent = std::move(p);
    return *this;
}

shared_ptr<Context> ContextBuilder::build() {
    return std::make_shared<Context>(Context{parent, std::move(bindings)});
}

Context::Context(shared_ptr<Context> parent) : parent(std::move(parent)) {}
Context::Context(shared_ptr<Context> parent, variables&& vars)
    : parent(std::move(parent)), bindings(std::move(vars)) {}

shared_ptr<Expr> Context::get(const string& name) const
{
    if (bindings.contains(name))
    {
        return bindings.at(name);
    }
    if (parent != nullptr)
    {
        return parent->get(name);
    }
    return nullptr;
}

bool Context::has(const string& name) const
{
    if (bindings.contains(name))
    {
        return true;
    }
    if (parent != nullptr)
    {
        return parent->has(name);
    }
    return false;
}


