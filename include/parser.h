//
// Created by glom on 9/25/25.
//

#ifndef GLOM_PARSER_H
#define GLOM_PARSER_H

#include "expr.h"

vector<unique_ptr<Expr>> parse(const string& input);


#endif //GLOM_PARSER_H