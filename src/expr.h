//
// Created by glom on 9/25/25.
//

#ifndef GLOM_EXPR_H
#define GLOM_EXPR_H
#ifndef EXPR_H
#define EXPR_H

#include <vector>
#include <string>
#include <memory>
#include <variant>

using std::string;
using std::vector;
using std::unique_ptr;

enum ExprType
{
    NUMBER,
    STRING,
    BOOLEAN,
    SYMBOL,
    LIST
};

/**
 * Expr class hierarchy representing different types of expressions.
 * - Number: Represents a numeric value.
 * - String: Represents a string value.
 * - Boolean: Represents a boolean value.
 * - Symbol: Represents an identifier (i.e. variable name).
 * - List: Represents a list of expressions.
 */
struct Expr {
    ExprType type;
    std::variant<
        double,
        string,
        bool,
        vector<unique_ptr<Expr>>
    > value;

    explicit Expr(double v);
    explicit Expr(bool v);
    explicit Expr(vector<unique_ptr<Expr>>& v);
    Expr(ExprType type, string v);

    [[nodiscard]] string toString() const
    {
        switch (type)
        {
            case NUMBER:
                return std::to_string(std::get<double>(value));
            case STRING:
                return "\"" + std::get<string>(value) + "\"";
            case BOOLEAN:
                return std::get<bool>(value) ? "true" : "false";
            case SYMBOL:
                return std::get<string>(value);
            case LIST: {
                const auto& list = std::get<vector<unique_ptr<Expr>>>(value);
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
            default:
                return "Unknown";
        }
    }
};

#endif
#endif //GLOM_EXPR_H