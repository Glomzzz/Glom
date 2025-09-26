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
    [[nodiscard]] ExprType getType() const;
    [[nodiscard]] double asNumber() const;
    [[nodiscard]] const string& asString() const;
    [[nodiscard]] bool asBoolean() const;
    [[nodiscard]] const vector<shared_ptr<Expr>>& asList() const;
    [[nodiscard]] shared_ptr<Lambda> asLambda() const;
    [[nodiscard]] shared_ptr<Primitive> asPrimitive() const;
    [[nodiscard]] string toString() const;

    static Expr makeNumber(double v);
    static Expr makeString(string v);
    static Expr makeBoolean(bool v);
    static Expr makeSymbol(string v);
    static Expr makeList(vector<shared_ptr<Expr>> v);
    static Expr makeLambda(shared_ptr<Lambda> v);
    static Expr makePrimitive(shared_ptr<Primitive> v);
};



class Param
{
    string name;
    bool vararg = false;
public:
    explicit Param(string name, bool vararg = false);
    [[nodiscard]] const string& getName() const;
    [[nodiscard]] bool isVararg() const;
    [[nodiscard]] string toString() const;
};

class Lambda
{
    vector<Param> params;
    vector<shared_ptr<Expr>> body;
    shared_ptr<Context> context;
public:
    Lambda(vector<Param>&& params, vector<shared_ptr<Expr>>&& body, shared_ptr<Context> context);
    [[nodiscard]] shared_ptr<Context> getContext();
    [[nodiscard]] const vector<Param>& getParams() const;
    [[nodiscard]] vector<shared_ptr<Expr>>& getBody();
    [[nodiscard]] string toString() const;
};

using PrimitiveProc = std::function<shared_ptr<Expr>(vector<shared_ptr<Expr>>&&)>;

class Primitive {
    PrimitiveProc proc;
    string name;

    explicit Primitive(PrimitiveProc proc, string name);
public:

    shared_ptr<Expr> operator()(vector<shared_ptr<Expr>>&& args) const;

    [[nodiscard]] const string& getName() const;
};

#endif //GLOM_EXPR_H