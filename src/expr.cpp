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

Expr Expr::make_boolean(const bool v)
{
    return Expr(v);
}
Expr Expr::make_number(const double v)
{
    return Expr(v);
}
Expr Expr::make_string(string v)
{
    return Expr(STRING, std::move(v));
}
Expr Expr::make_symbol(string v)
{
    return Expr(SYMBOL, std::move(v));
}

Expr Expr::make_list(vector<shared_ptr<Expr>> v)
{
    return Expr(std::move(v));
}
Expr Expr::make_lambda(shared_ptr<Lambda> v)
{
    return Expr(std::move(v));
}
Expr Expr::make_primitive(shared_ptr<Primitive> v)
{
    return Expr(std::move(v));
}


ExprType Expr::get_type() const
{
    return type;
}
bool Expr::as_boolean() const
{
    return std::get<bool>(value);
}
double Expr::as_number() const
{
    return std::get<double>(value);
}
const string& Expr::as_string() const
{
    return std::get<string>(value);
}
const vector<shared_ptr<Expr>>& Expr::as_list() const
{
    return std::get<vector<shared_ptr<Expr>>>(value);
}
shared_ptr<Lambda> Expr::as_lambda() const
{
    return std::get<shared_ptr<Lambda>>(value);
}
shared_ptr<Primitive> Expr::as_primitive() const
{
    return std::get<shared_ptr<Primitive>>(value);
}


Param::Param(string name, const bool vararg) : name(std::move(name)), vararg(vararg) {}

bool Param::is_vararg() const {
    return vararg;
}
const string& Param::get_name() const
{
    return name;
}
string Param::to_string() const
{
    return name + (vararg ? "..." : "");
}



Lambda::Lambda(vector<Param>&& params, vector<shared_ptr<Expr>>&& body, shared_ptr<Context> context)
    : params(std::move(params)), body(std::move(body)), context(std::move(context)) {}

vector<shared_ptr<Expr>>& Lambda::get_body()
{
    return body;
}

shared_ptr<Context> Lambda::get_context()
{
    return context;
}

const vector<Param>& Lambda::get_params() const
{
    return params;
}

string Lambda::to_string() const
{
    string result = "(lambda (";
    for (size_t i = 0; i < params.size(); ++i)
    {
        result += params[i].to_string();
        if (i < params.size() - 1)
            result += " ";
    }
    result += ")";
    for (const auto& expr : body)
    {
        result += " " + expr->to_string();
    }
    result += ")";
    return result;
}

Primitive::Primitive(string name, PrimitiveProc proc) : name(std::move(name)),proc(std::move(proc)) {}

shared_ptr<Expr> Primitive::operator()(Context* context, vector<shared_ptr<Expr>>&& args) const
{
    return proc(context, std::move(args));
}

const string& Primitive::get_name() const
{
    return name;
}


string Expr::to_string() const
{
    switch (type)
    {
        case NUMBER:
            return std::to_string(as_number());
        case STRING:
            return "\"" + as_string() + "\"";
        case BOOLEAN:
            return as_boolean() ? "true" : "false";
        case SYMBOL:
            return as_string();
        case LIST:
            {
                const auto& list = as_list();
                string result = "(";
                for (size_t i = 0; i < list.size(); ++i)
                {
                    result += list[i]->to_string();
                    if (i < list.size() - 1)
                        result += " ";
                }
                result += ")";
                return result;
            }
        case LAMBDA:
            return as_lambda()->to_string();
        case PRIMITIVE:
            return "<primitive:" + as_primitive()->get_name();
        default:
            return "Unknown";
    }
}





