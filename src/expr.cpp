#include <utility>
#include <vector>
#include <string>
#include <memory>

#include "expr.h"

using std::string;
using std::vector;
using std::shared_ptr;

Expr::Expr(double v) :type(NUMBER), value(v) {}
Expr::Expr(bool v) : type(BOOLEAN),value(v) {}

Expr::Expr(vector<shared_ptr<Expr>>&& v) :type(LIST), value(std::move(v)) {}
Expr::Expr(shared_ptr<Lambda>&& v) :type(LAMBDA), value(std::move(v)) {}
Expr::Expr(shared_ptr<Primitive>&& v) :type(PRIMITIVE), value(std::move(v)) {}

Expr::Expr(const ExprType type, string&& v) :type(type), value(std::move(v)) {}

Expr Expr::makeBoolean(const bool v)
{
    return Expr(v);
}
Expr Expr::makeNumber(const double v)
{
    return Expr(v);
}
Expr Expr::makeString(string v)
{
    return Expr(STRING, std::move(v));
}
Expr Expr::makeSymbol(string v)
{
    return Expr(SYMBOL, std::move(v));
}

Expr Expr::makeList(vector<shared_ptr<Expr>> v)
{
    return Expr(std::move(v));
}
Expr Expr::makeLambda(shared_ptr<Lambda> v)
{
    return Expr(std::move(v));
}
Expr Expr::makePrimitive(shared_ptr<Primitive> v)
{
    return Expr(std::move(v));
}


ExprType Expr::getType() const
{
    return type;
}
bool Expr::asBoolean() const
{
    return std::get<bool>(value);
}
double Expr::asNumber() const
{
    return std::get<double>(value);
}
const string& Expr::asString() const
{
    return std::get<string>(value);
}
const vector<shared_ptr<Expr>>& Expr::asList() const
{
    return std::get<vector<shared_ptr<Expr>>>(value);
}
shared_ptr<Lambda> Expr::asLambda() const
{
    return std::get<shared_ptr<Lambda>>(value);
}
shared_ptr<Primitive> Expr::asPrimitive() const
{
    return std::get<shared_ptr<Primitive>>(value);
}


Param::Param(string name, const bool vararg) : name(std::move(name)), vararg(vararg) {}

bool Param::isVararg() const {
    return vararg;
}
const string& Param::getName() const
{
    return name;
}
string Param::toString() const
{
    return name + (vararg ? "..." : "");
}



Lambda::Lambda(vector<Param>&& params, vector<shared_ptr<Expr>>&& body, shared_ptr<Context> context)
    : params(std::move(params)), body(std::move(body)), context(std::move(context)) {}

vector<shared_ptr<Expr>>& Lambda::getBody()
{
    return body;
}

shared_ptr<Context> Lambda::getContext()
{
    return context;
}

const vector<Param>& Lambda::getParams() const
{
    return params;
}

string Lambda::toString() const
{
    string result = "(lambda (";
    for (size_t i = 0; i < params.size(); ++i)
    {
        result += params[i].toString();
        if (i < params.size() - 1)
            result += " ";
    }
    result += ")";
    for (const auto& expr : body)
    {
        result += " " + expr->toString();
    }
    result += ")";
    return result;
}

Primitive::Primitive(PrimitiveProc proc, string name) : proc(std::move(proc)), name(std::move(name)) {}

shared_ptr<Expr> Primitive::operator()(vector<shared_ptr<Expr>>&& args) const
{
    return proc(std::move(args));
}

const string& Primitive::getName() const
{
    return name;
}


string Expr::toString() const
{
    switch (type)
    {
        case NUMBER:
            return std::to_string(asNumber());
        case STRING:
            return "\"" + asString() + "\"";
        case BOOLEAN:
            return asBoolean() ? "true" : "false";
        case SYMBOL:
            return asString();
        case LIST:
            {
                const auto& list = asList();
                string result = "(";
                for (size_t i = 0; i < list.size(); ++i)
                {
                    result += list[i]->toString();
                    if (i < list.size() - 1)
                        result += " ";
                }
                result += ")";
                return result;
            }
        case LAMBDA:
            return asLambda()->toString();
        case PRIMITIVE:
            return "<primitive:" + asPrimitive()->getName();
        default:
            return "Unknown";
    }
}





