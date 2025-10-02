//
// Created by glom on 9/25/25.
//

#ifndef GLOM_PARSER_H
#define GLOM_PARSER_H

#include <vector>
#include <string>
#include <memory>

using std::string;
using std::vector;
using std::unique_ptr;
using std::shared_ptr;

class Expr;
class Pair;

shared_ptr<Expr> parse_expr(string input);
shared_ptr<Pair> parse(string input);


#endif //GLOM_PARSER_H