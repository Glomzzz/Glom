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
Expr::Expr(vector<unique_ptr<Expr>>& v) :type(LIST), value(std::move(v)) {}
Expr::Expr(const ExprType type, string v) :type(type), value(std::move(v)) {}