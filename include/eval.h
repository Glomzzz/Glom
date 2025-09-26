//
// Created by glom on 9/26/25.
//

#ifndef GLOM_EVAL_H
#define GLOM_EVAL_H
#include <stdexcept>
#include <string>

class EvalError : public std::runtime_error {
public:
    explicit EvalError(const std::string& msg);
};

#endif //GLOM_EVAL_H