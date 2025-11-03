//
// Created by glom on 10/24/25.
//

#ifndef GLOM_MODULE_H
#define GLOM_MODULE_H
#include "context.h"

class Module
{
    string id;
    shared_ptr<const Context> exports;
public:
    explicit Module(string name);

    [[nodiscard]] const string& get_name() const;
    [[nodiscard]] shared_ptr<const Context> get_exports() const;
    static Module load_from_file(const string& path);
};

#endif //GLOM_MODULE_H