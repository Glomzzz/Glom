//
// Created by glom on 9/26/25.
//

#ifndef GLOM_ERROR_H
#define GLOM_ERROR_H
#include <stdexcept>
#include <string>

class GlomError : public std::runtime_error {
public:
    explicit GlomError(const std::string& msg);
};

#endif //GLOM_ERROR_H