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
class Param;

typedef unordered_map<string, shared_ptr<Expr>> variables;
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
    shared_ptr<Expr> get(const string& name) const;
    bool has(const string& name) const;
    Context& operator=(const Context&) = delete;

    void set_parent(shared_ptr<Context> new_parent);
    void add(const string& name, shared_ptr<Expr> value);
    void add_primitive(const string& name, PrimitiveProc proc);

    shared_ptr<Expr> eval(shared_ptr<Expr> expr);
    shared_ptr<Expr> eval(const shared_ptr<Pair>& exprs);
    shared_ptr<Expr> eval(const vector<shared_ptr<Expr>>& exprs);

    shared_ptr<Context> eval_apply_context(const shared_ptr<Expr>& proc, shared_ptr<Context> current_parent,
                                                  const vector<Param>& params, shared_ptr<Pair>&& args);
    shared_ptr<Expr> apply(shared_ptr<Expr>&& proc,
                           shared_ptr<Pair>&& args);

    string to_string() const;

    static shared_ptr<Context> new_context();
    static shared_ptr<Context> new_context(shared_ptr<Context> parent);
    static shared_ptr<Context> new_context(shared_ptr<Context> parent, variables&& bindings);
};


shared_ptr<Context> make_root_context();
#endif //GLOM_CONTEXT_H
