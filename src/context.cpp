//
// Created by glom on 9/26/25.
//
#include "context.h"
#include <utility>
#include "expr.h"



Context::Context(shared_ptr<Context> parent, variables&& vars)
    : parent(std::move(parent)), bindings(std::move(vars)) {}

void Context::add(const string_view name, shared_ptr<Expr> value) {
    bindings[name] = std::move(value);
}


void Context::set_parent(shared_ptr<Context> new_parent) {
    parent = std::move(new_parent);
}

shared_ptr<Context> Context::new_context() {
    return std::make_shared<Context>(Context{nullptr,{}});
}

shared_ptr<Context> Context::new_context(shared_ptr<Context> parent) {
    return std::make_shared<Context>(Context{std::move(parent), {}});
}
shared_ptr<Context> Context::new_context(shared_ptr<Context> parent, variables&& bindings) {
    return std::make_shared<Context>(Context{std::move(parent), std::move(bindings)});
}

shared_ptr<Expr> Context::get(const string_view& name) const
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

bool Context::has(const string_view& name) const
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

string Context::to_string() const
{
    string result = "{";
    bool first = true;
    for (const auto& [key, value] : bindings)
    {
        if (!first)
        {
            result += ", ";
        }
        result += view_to_string(key) + ": " + value->to_string();
        first = false;
    }
    result += "}";
    return result;
}


