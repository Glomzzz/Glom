//
// Created by glom on 9/26/25.
//
#include "error.h"


GlomError::GlomError(const std::string& msg) : std::runtime_error(msg) {}