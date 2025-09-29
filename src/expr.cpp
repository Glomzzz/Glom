#include <utility>
#include <vector>
#include <string>
#include <memory>

#include "expr.h"
#include "primitive.h"

using std::string;
using std::vector;
using std::shared_ptr;

const shared_ptr<Pair> Pair::EMPTY = std::make_shared<Pair>(Pair());

Pair::Pair(): data(nullptr), next(Expr::NIL) {}
Pair::Pair(shared_ptr<Expr> expr) :data(std::move(expr)), next(Expr::NIL) {}
Pair::Pair(shared_ptr<Expr> expr, shared_ptr<Expr> next) :data(std::move(expr)), next(std::move(next)) {}

shared_ptr<Pair> Pair::single(shared_ptr<Expr> car)
{
    return cons(std::move(car), Expr::NIL);
}
shared_ptr<Pair> Pair::cons(shared_ptr<Expr> car, shared_ptr<Expr> cdr)
{
    return std::make_shared<Pair>(Pair(std::move(car), std::move(cdr)));
}

Pair::iterator::iterator(shared_ptr<Pair> pair): current(std::move(pair)) {}

shared_ptr<Expr> Pair::iterator::operator*() const
{
    return current ? current->car() : nullptr;
}

Pair::iterator& Pair::iterator::operator++()
{
    if (current && current->cdr()) {
        auto next_expr = current->cdr();
        if (next_expr->get_type() == PAIR) {
            current = next_expr->as_pair();
        } else {
            current = single(std::move(next_expr));
        }
    } else {
        current = nullptr;
    }
    return *this;
}

Pair::iterator Pair::iterator::operator++(int)
{
    iterator temp = *this;
    ++(*this);
    return temp;
}

bool Pair::iterator::operator==(const iterator& other) const
{
    return current == other.current;
}

bool Pair::iterator::operator!=(const iterator& other) const
{
    return !(*this == other);
}

shared_ptr<Expr> Pair::car() const
{
    return data;
}
shared_ptr<Expr> Pair::cdr() const
{
    return next;
}
void Pair::set_car(shared_ptr<Expr> car)
{
    this->data = std::move(car);
}
void Pair::set_cdr(shared_ptr<Expr> cdr)
{
    this->next = std::move(cdr);
}

bool Pair::empty() const
{
    return data == nullptr;
}



string Pair::to_string() const
{
    if (empty())
        return "()";
    string result = "(";
    shared_ptr<Expr> current = data;

    result += current->to_string();
    current = next;

    while (current && current->get_type() == PAIR)
    {
        const auto pair = current->as_pair();
        if (pair->empty())
        {
            current = nullptr;
            break;
        }
        result += " ";
        result += pair->car()->to_string();
        current = pair->cdr();
    }
    if (current)
        result += " . " + current->to_string();
    result += ")";
    return result;
}


Expr::Expr(const ExprType type):type(type) {}
Expr::Expr(const double v) :type(NUMBER), value(v) {}
Expr::Expr(const bool v) : type(BOOLEAN),value(v) {}

Expr::Expr(shared_ptr<Pair>&& v) :type(PAIR), value(std::move(v)) {}
Expr::Expr(shared_ptr<Lambda>&& v) :type(LAMBDA), value(std::move(v)) {}
Expr::Expr(shared_ptr<Primitive>&& v) :type(PRIMITIVE), value(std::move(v)) {}

Expr::Expr(string&& v) :type(STRING), value(std::move(v)) {}
Expr::Expr(const string_view v): type(SYMBOL), value(v) {}




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
const string_view& Expr::as_symbol() const
{
    return std::get<string_view>(value);
}

shared_ptr<Pair> Expr::as_pair() const
{
    return std::get<shared_ptr<Pair>>(value);
}
shared_ptr<Lambda> Expr::as_lambda() const
{
    return std::get<shared_ptr<Lambda>>(value);
}
shared_ptr<Primitive> Expr::as_primitive() const
{
    return std::get<shared_ptr<Primitive>>(value);
}


Param::Param(const string_view name, const bool vararg) : name(name), vararg(vararg) {}

bool Param::is_vararg() const {
    return vararg;
}
const string_view& Param::get_name() const
{
    return name;
}
string Param::to_string() const
{
    string result = view_to_string(name);
    if (vararg)
        result = ". " + result;
    return result;
}

SymbolPool& SymbolPool::instance() {
    static SymbolPool inst;
    return inst;
}

string_view SymbolPool::intern(const string& s) {
    std::lock_guard lk(mutex);
    auto [it, inserted] = pool.emplace(s);
    return string_view(*it);
}

string_view SymbolPool::intern(string&& s) {
    std::lock_guard lk(mutex);
    auto [it, inserted] = pool.emplace(std::move(s));
    return string_view(*it);
}

size_t SymbolPool::size() const {
    return pool.size();
}

string view_to_string(const string_view& view) {
    return {view.data(), view.size()};
}

const std::shared_ptr<Expr> Expr::TRUE = std::make_shared<Expr>(true);
const std::shared_ptr<Expr> Expr::FALSE = std::make_shared<Expr>(false);
const std::shared_ptr<Expr> Expr::NOTHING = std::make_shared<Expr>(VOID);
const std::shared_ptr<Expr> Expr::NIL = make_pair(Pair::EMPTY);

shared_ptr<Expr> Expr::make_number(const double v)
{
    return std::make_shared<Expr>(v);
}
shared_ptr<Expr> Expr::make_boolean(const bool cond)
{
    return cond ? TRUE : FALSE;
}
shared_ptr<Expr> Expr::make_string(string v)
{
    return std::make_shared<Expr>(Expr(std::move(v)));
}
shared_ptr<Expr> Expr::make_symbol(string v)
{
    auto str_view = SymbolPool::instance().intern(std::move(v));
    return std::make_shared<Expr>(Expr(str_view));
}
shared_ptr<Expr> Expr::make_lambda(shared_ptr<Lambda> v)
{
    return std::make_shared<Expr>(Expr(std::move(v)));
}
shared_ptr<Expr> Expr::make_primitive(shared_ptr<Primitive> v)
{
    return std::make_shared<Expr>(Expr(std::move(v)));
}
shared_ptr<Expr> Expr::make_pair(shared_ptr<Pair> v)
{
    return std::make_shared<Expr>(Expr(std::move(v)));
}


Lambda::Lambda(vector<Param>&& params, shared_ptr<Pair> body, shared_ptr<Context> context)
    : params(std::move(params)), body(std::move(body)), context(std::move(context)) {}

shared_ptr<Pair> Lambda::get_body()
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
    for (const auto& expr : *body)
    {
        if (!expr) break;
        result += " " + expr->to_string();
    }
    result += ")";
    return result;
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
            return view_to_string(as_symbol());
        case PAIR:
            return as_pair()->to_string();
        case LAMBDA:
            return as_lambda()->to_string();
        case PRIMITIVE:
            return "<primitive:" + as_primitive()->get_name();
        default:
            return "Unknown";
    }
}





