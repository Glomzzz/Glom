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

vector<shared_ptr<Expr>> parse(string&& input);
vector<shared_ptr<Expr>> parse(const string& input);


#endif //GLOM_PARSER_H