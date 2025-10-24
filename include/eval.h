//
// Created by glom on 10/1/25.
//

#ifndef GLOM_EVAL_H
#define GLOM_EVAL_H
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
class Param;
class Pair;
class Context;
struct Continuation;

class GlomCont : public std::exception {
public:
    unique_ptr<Continuation> cont;
    shared_ptr<Expr> value;
    explicit GlomCont(unique_ptr<Continuation> cont, shared_ptr<Expr> value);
};

shared_ptr<Expr> eval(const shared_ptr<Context>& ctx, shared_ptr<Expr> expr);
shared_ptr<Expr> eval(const shared_ptr<Context>& ctx, shared_ptr<Pair> rest);

shared_ptr<Context> eval_apply_context(const shared_ptr<Context>& ctx, const shared_ptr<Expr>& proc, const shared_ptr<Context>& current_parent,
                                              const vector<Param>& params, shared_ptr<Pair>&& args);

#endif //GLOM_EVAL_H