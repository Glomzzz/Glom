#include <utility>
#include <vector>
#include <string>
#include <memory>

#include "expr.h"

#include "error.h"
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
        if (auto next_expr = current->cdr(); next_expr->is_pair()) {
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

    while (current && current->is_pair())
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


Expr::Expr() : value(monostate{}) {}
Expr::Expr(const bool v) : value(v) {}
Expr::Expr(integer v) : value(std::move(v)) {}
Expr::Expr(const real v) : value(v) {}
Expr::Expr(rational v) : value(std::make_unique<rational>(std::move(v))) {}

Expr::Expr(shared_ptr<Pair>&& v) : value(std::move(v)) {}
Expr::Expr(shared_ptr<Lambda>&& v) : value(std::move(v)) {}
Expr::Expr(shared_ptr<Primitive>&& v) : value(std::move(v)) {}
Expr::Expr(std::unique_ptr<string>&& v) : value(std::move(v)) {}
Expr::Expr(std::unique_ptr<Continuation>&& v) : value(std::move(v)) {}
Expr::Expr(const string_view v): value(v) {}


ExprType Expr::get_type() const
{
    return value.index();
}
bool Expr::is_nothing() const
{
    return value.index() == VOID;
}
bool Expr::is_number() const
{
    const auto index = value.index();
    return index >= NUMBER_INT && index <= NUMBER_REAL;
}
bool Expr::is_number_int() const
{
    return value.index() == NUMBER_INT;
}
bool Expr::is_number_rat() const
{
    return value.index() == NUMBER_RAT;
}
bool Expr::is_number_real() const
{
    return value.index() == NUMBER_REAL;
}
bool Expr::is_string() const
{
    return value.index() == STRING;
}
bool Expr::is_symbol() const
{
    return value.index() == SYMBOL;
}
bool Expr::is_boolean() const
{
    return value.index() == BOOLEAN;
}
bool Expr::is_pair() const
{
    return value.index() == PAIR;
}
bool Expr::is_lambda() const
{
    return value.index() == LAMBDA;
}
bool Expr::is_primitive() const
{
    return value.index() == PRIMITIVE;
}
bool Expr::is_cont() const
{
    return value.index() == CONTINUATION;
}

bool Expr::as_boolean() const
{
    return std::get<bool>(value);
}
integer Expr::as_number_int() const
{
    return std::get<integer>(value);
}
const rational& Expr::as_number_rat() const
{
    return *std::get<unique_ptr<rational>>(value);
}

real Expr::as_number_real() const
{
    return std::get<real>(value);
}
real Expr::to_number_real() const
{
    switch (value.index())
    {
        case NUMBER_INT:
            return as_number_int().to_real();
        case NUMBER_RAT:
        {
            auto& [num, den] = as_number_rat();
            return num.to_real() / den.to_real();
        }
        case NUMBER_REAL:
            return as_number_real();
        default:
            throw GlomError("Cannot convert to real: " + to_string());
    }
}

rational Expr::to_number_rat() const
{
    switch (value.index())
    {
        case NUMBER_INT:
            return as_number_int().to_rational();
        case NUMBER_RAT:
            return as_number_rat();
        case NUMBER_REAL:
            throw GlomError("Cannot convert real to rational: " + to_string());
        default:
            throw GlomError("Cannot convert to rational: " + to_string());
    }
}

const string& Expr::as_string() const
{
    return *std::get<unique_ptr<string>>(value);
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

Continuation& Expr::as_cont() const
{
    return *std::get<std::unique_ptr<Continuation>>(value);
}

bool Expr::to_boolean() const
{
    if (!is_boolean())
    {
        return true;
    }
    return as_boolean();
}

bool Expr::is_nil() const
{
    return is_pair() && as_pair()->empty();
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
const std::shared_ptr<Expr> Expr::NOTHING = std::make_shared<Expr>();
const std::shared_ptr<Expr> Expr::NIL = make_pair(Pair::EMPTY);

shared_ptr<Expr> Expr::make_number_int(integer v)
{
    return std::make_shared<Expr>(Expr(std::move(v)));
}

shared_ptr<Expr> Expr::make_number_rat(rational rat)
{
    return std::make_shared<Expr>(Expr(std::move(rat)));
}

shared_ptr<Expr> Expr::make_number_exact(rational rat)
{
    if (rat.is_integer())
    {
        return make_number_int(std::move(rat.num));
    }
    return std::make_shared<Expr>(Expr(std::move(rat)));
}
shared_ptr<Expr> Expr::make_number_real(const real v)
{
    return std::make_shared<Expr>(Expr(v));
}
shared_ptr<Expr> Expr::make_boolean(const bool cond)
{
    return cond ? TRUE : FALSE;
}
shared_ptr<Expr> Expr::make_string(unique_ptr<string> v)
{
    return std::make_shared<Expr>(Expr(std::move(v)));
}
shared_ptr<Expr> Expr::make_symbol(string v)
{
    const auto str_view = SymbolPool::instance().intern(std::move(v));
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
shared_ptr<Expr> Expr::make_cont(unique_ptr<Continuation> v)
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
    switch (value.index())
    {
        case NUMBER_INT:
            return as_number_int().to_decimal_string();
        case NUMBER_RAT:
        {
            return as_number_rat().to_rational_string();
        }
        case NUMBER_REAL:
            return std::to_string(as_number_real());
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
        case CONTINUATION:
            return "<continuation>";
        default:
            return "Unknown";
    }
}





