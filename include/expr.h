//
// Created by glom on 9/25/25.
//

#ifndef GLOM_EXPR_H
#define GLOM_EXPR_H

#include <vector>
#include <string>
#include <memory>
#include <variant>
#include <unordered_set>
#include <shared_mutex>
#include "primitive.h"

using std::string;
using std::string_view;
using std::vector;
using std::unique_ptr;
using std::shared_ptr;
using std::monostate;


enum ExprType
{
    NUMBER,
    STRING,
    BOOLEAN,
    SYMBOL,
    PAIR,
    LAMBDA,
    PRIMITIVE,
    VOID
};

class Context;
class Lambda;

class Pair : public std::enable_shared_from_this<Pair>
{
    shared_ptr<Expr> data;
    shared_ptr<Expr> next;

    Pair();
    explicit Pair(shared_ptr<Expr> expr);
    explicit Pair(shared_ptr<Expr> expr, shared_ptr<Expr> next);
public:
    static const shared_ptr<Pair> EMPTY;
    [[nodiscard]] shared_ptr<Expr> car() const;
    [[nodiscard]] shared_ptr<Expr> cdr() const;
    [[nodiscard]] string to_string() const;
    [[nodiscard]] bool empty() const;
    void set_car(shared_ptr<Expr> car);
    void set_cdr(shared_ptr<Expr> cdr);

    static shared_ptr<Pair> single(shared_ptr<Expr> car);
    static shared_ptr<Pair> cons(shared_ptr<Expr> car, shared_ptr<Expr> cdr);

    class iterator {
        shared_ptr<Pair> current;
    public:
        explicit iterator(shared_ptr<Pair> pair = nullptr);

        shared_ptr<Expr> operator*() const;

        iterator& operator++();

        iterator operator++(int);

        bool operator==(const iterator& other) const;

        bool operator!=(const iterator& other) const;
    };

    iterator begin() {
        return empty() ? end() : iterator(shared_from_this());
    }

    iterator end() {
        return iterator(nullptr);
    }
};


class SymbolPool {
    std::unordered_set<string> pool;
    std::shared_mutex mutex;

public:
    static SymbolPool& instance();

    string_view intern(const string& s);

    string_view intern(string&& s);

    size_t size() const;
};

string view_to_string(const string_view& view);

using ExprValue = std::variant<
    std::monostate,
    double,                 // number
    std::string,            // string literal/value
    std::string_view,       // symbol (interned)
    bool,
    std::shared_ptr<Pair>,
    std::shared_ptr<Lambda>,
    std::shared_ptr<Primitive>
>;


/**
 * Expr class hierarchy representing different types of expressions.
 * - Number: Represents a numeric value.
 * - String: Represents a string value.
 * - Boolean: Represents a boolean value.
 * - Symbol: Represents an identifier (i.e. variable name).
 * - Pair: Represents a pair of expressions.
 * - Lambda: Represents a lambda function. (Only constructed in runtime)
 * - Primitive: Represents a primitive members. (Only constructed in c++)
 * - None: Represents None.
 */
class Expr {

    ExprType type;
    ExprValue value;
    explicit Expr(shared_ptr<Pair>&& v);
    explicit Expr(shared_ptr<Lambda>&& v);
    explicit Expr(shared_ptr<Primitive>&& v);
    explicit Expr(string&& v);
    explicit Expr(string_view v);
public:
    explicit Expr(bool v);
    explicit Expr(double v);
    explicit Expr(ExprType type);
    [[nodiscard]] ExprType get_type() const;
    [[nodiscard]] double as_number() const;
    [[nodiscard]] const string& as_string() const;
    [[nodiscard]] const string_view& as_symbol() const;
    [[nodiscard]] bool as_boolean() const;
    [[nodiscard]] shared_ptr<Pair> as_pair() const;
    [[nodiscard]] shared_ptr<Lambda> as_lambda() const;
    [[nodiscard]] shared_ptr<Primitive> as_primitive() const;
    [[nodiscard]] string to_string() const;
    [[nodiscard]] bool to_boolean() const;
    [[nodiscard]] bool is_nil() const;
    static const  shared_ptr<Expr> TRUE;
    static const  shared_ptr<Expr> FALSE;
    static const  shared_ptr<Expr> NIL;
    static const  shared_ptr<Expr> NOTHING;
    static shared_ptr<Expr> make_number(double v);
    static shared_ptr<Expr> make_boolean(bool cond);
    static shared_ptr<Expr> make_string(string v);
    static shared_ptr<Expr> make_symbol(string v);
    static shared_ptr<Expr> make_lambda(shared_ptr<Lambda> v);
    static shared_ptr<Expr> make_primitive(shared_ptr<Primitive> v);
    static shared_ptr<Expr> make_pair(shared_ptr<Pair> v);
};



class Param
{
    string_view name;
    bool vararg = false;
public:
    explicit Param(string_view name, bool vararg = false);
    [[nodiscard]] const string_view& get_name() const;
    [[nodiscard]] bool is_vararg() const;
    [[nodiscard]] string to_string() const;
};

class Lambda
{
    vector<Param> params;
    shared_ptr<Pair> body;
    shared_ptr<Context> context;
public:
    Lambda(vector<Param>&& params, shared_ptr<Pair> body, shared_ptr<Context> context);
    [[nodiscard]] const vector<Param>& get_params() const;
    [[nodiscard]] shared_ptr<Context> get_context();
    [[nodiscard]] shared_ptr<Pair> get_body();
    [[nodiscard]] string to_string() const;
};



#endif //GLOM_EXPR_H