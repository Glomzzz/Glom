//
// Created by glom on 9/26/25.
//

#ifndef GLOM_PRIMITIVE_H
#define GLOM_PRIMITIVE_H

#include <functional>
#include <vector>
#include <string>
#include <memory>

class Pair;
class Context;
class Expr;

using std::string;
using std::vector;
using std::unique_ptr;
using std::shared_ptr;
using PrimitiveProc = std::function<shared_ptr<Expr>(shared_ptr<Context>, shared_ptr<Pair>&&)>;

class Primitive {
    string name;
    PrimitiveProc proc;
public:
    explicit Primitive(string name, PrimitiveProc proc);

    shared_ptr<Expr> operator()(const shared_ptr<Context>& context, shared_ptr<Pair>&& args) const;

    [[nodiscard]] const string& get_name() const;
};

#endif //GLOM_PRIMITIVE_H