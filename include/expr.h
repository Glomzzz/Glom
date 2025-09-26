//
// Created by glom on 9/25/25.
//

#ifndef GLOM_EXPR_H
#define GLOM_EXPR_H

#include <context.h>
#include <functional>
#include <vector>
#include <string>
#include <memory>
#include <variant>

using std::string;
using std::vector;
using std::unique_ptr;
using std::shared_ptr;

enum ExprType
{
    NUMBER,
    STRING,
    BOOLEAN,
    SYMBOL,
    LIST,
    LAMBDA,
    PRIMITIVE
};

class Lambda;
class Primitive;

/**
 * Expr class hierarchy representing different types of expressions.
 * - Number: Represents a numeric value.
 * - String: Represents a string value.
 * - Boolean: Represents a boolean value.
 * - Symbol: Represents an identifier (i.e. variable name).
 * - List: Represents a list of expressions.
 * - Lambda: Represents a lambda function.
 * - Primitive: Represents a primitive members.
 */
class Expr {
    ExprType type;
    std::variant<
        double,
        string,
        bool,
        vector<shared_ptr<Expr>>,
        shared_ptr<Lambda>,
        shared_ptr<Primitive>
    > value;
private:
    explicit Expr(double v);
    explicit Expr(bool v);
    explicit Expr(vector<shared_ptr<Expr>>&& v);
    explicit Expr(shared_ptr<Lambda>&& v);
    explicit Expr(shared_ptr<Primitive>&& v);
    Expr(ExprType type, string&& v);
public:
    [[nodiscard]] ExprType get_type() const;
    [[nodiscard]] double as_number() const;
    [[nodiscard]] const string& as_string() const;
    [[nodiscard]] bool as_boolean() const;
    [[nodiscard]] const vector<shared_ptr<Expr>>& as_list() const;
    [[nodiscard]] shared_ptr<Lambda> as_lambda() const;
    [[nodiscard]] shared_ptr<Primitive> as_primitive() const;
    [[nodiscard]] string to_string() const;

    static Expr make_number(double v);
    static Expr make_string(string v);
    static Expr make_boolean(bool v);
    static Expr make_symbol(string v);
    static Expr make_list(vector<shared_ptr<Expr>> v);
    static Expr make_lambda(shared_ptr<Lambda> v);
    static Expr make_primitive(shared_ptr<Primitive> v);
};



class Param
{
    string name;
    bool vararg = false;
public:
    explicit Param(string name, bool vararg = false);
    [[nodiscard]] const string& get_name() const;
    [[nodiscard]] bool is_vararg() const;
    [[nodiscard]] string to_string() const;
};

class Lambda
{
    vector<Param> params;
    vector<shared_ptr<Expr>> body;
    shared_ptr<Context> context;
public:
    Lambda(vector<Param>&& params, vector<shared_ptr<Expr>>&& body, shared_ptr<Context> context);
    [[nodiscard]] shared_ptr<Context> get_context();
    [[nodiscard]] const vector<Param>& get_params() const;
    [[nodiscard]] vector<shared_ptr<Expr>>& get_body();
    [[nodiscard]] string to_string() const;
};

using PrimitiveProc = std::function<shared_ptr<Expr>(vector<shared_ptr<Expr>>&&)>;

class Primitive {
    PrimitiveProc proc;
    string name;

    explicit Primitive(PrimitiveProc proc, string name);
public:

    shared_ptr<Expr> operator()(vector<shared_ptr<Expr>>&& args) const;

    [[nodiscard]] const string& get_name() const;
};

#endif //GLOM_EXPR_H