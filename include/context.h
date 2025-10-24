//
// Created by glom on 9/26/25.
//

#ifndef GLOM_CONTEXT_H
#define GLOM_CONTEXT_H
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "primitive.h"

using std::string;
using std::vector;
using std::unique_ptr;
using std::shared_ptr;
using std::unordered_map;

class Expr;

typedef unordered_map<std::string_view, shared_ptr<Expr>> variables;
/**
 * Context for variable bindings.
 */
class Context : public std::enable_shared_from_this<Context>
{
    Context(shared_ptr<Context> parent, variables&& vars);

protected:
    shared_ptr<Context> parent = nullptr;
    variables bindings;
public:
    size_t depth;
    shared_ptr<Expr> get(const std::string_view& name) const;
    bool has(const std::string_view& name) const;
    Context& operator=(const Context&) = delete;

    void set_parent(shared_ptr<Context> new_parent);
    void add(std::string_view name, shared_ptr<Expr> value);
    void add_primitive(const string& name, PrimitiveProc proc);
    bool assign(const std::string_view& name, shared_ptr<Expr> value);

    string to_string() const;

    static shared_ptr<Context> new_context();
    static shared_ptr<Context> new_context(shared_ptr<Context> parent);
    static shared_ptr<Context> new_context(shared_ptr<Context> parent, variables&& bindings);
};


shared_ptr<Context> make_root_context();
#endif //GLOM_CONTEXT_H
