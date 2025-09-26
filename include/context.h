//
// Created by glom on 9/26/25.
//

#ifndef GLOM_CONTEXT_H
#define GLOM_CONTEXT_H
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

using std::string;
using std::vector;
using std::unique_ptr;
using std::shared_ptr;
using std::unordered_map;

class Expr;
class ContextBuilder;

typedef unordered_map<string, shared_ptr<Expr>> variables;
/**
 * Immutable context for variable bindings.
 */
class Context
{
friend ContextBuilder;
    Context(shared_ptr<Context> parent, variables&& vars);

protected:
    shared_ptr<Context> parent = nullptr;
    variables bindings;
public:
    explicit Context(shared_ptr<Context> parent);
    shared_ptr<Expr> get(const string& name) const;
    bool has(const string& name) const;

    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;
    Context(Context&&) = default;
    Context& operator=(Context&&) = default;

    virtual ~Context() = default;

    shared_ptr<Expr> eval(const shared_ptr<Expr>& expr);
    shared_ptr<Expr> eval(shared_ptr<Expr>&& expr);
    shared_ptr<Expr> eval(const vector<shared_ptr<Expr>>& exprs);
    shared_ptr<Expr> eval(vector<shared_ptr<Expr>>&& exprs);
private:
    template <class T>
    shared_ptr<Expr> evalImpl(T&& expr);
    static shared_ptr<Expr> apply(shared_ptr<Expr>&& proc, vector<shared_ptr<Expr>>&& args);
};


class ContextBuilder
{
    shared_ptr<Context> parent = nullptr;
    variables bindings;
public:
    explicit ContextBuilder(shared_ptr<Context> p);
    ContextBuilder& set(const string& name, shared_ptr<Expr> value);

    ContextBuilder& setParent(shared_ptr<Context> p);

    shared_ptr<Context> build();
};

#endif //GLOM_CONTEXT_H