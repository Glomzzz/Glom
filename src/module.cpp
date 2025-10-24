// module.cpp
#include <fstream>
#include <sstream>
#include <filesystem>
#include <utility>

#include "context.h"
#include "expr.h"
#include "parser.h"
#include "eval.h"
#include "error.h"
#include "module.h"

#include <ranges>

const string MODULE_KEY = "__module_exports_keys__";

const string EXPORTS_ALL_KEY = "__module_exports_all__";

void Context::init_module()
{
    add(MODULE_KEY, Expr::NIL);
}

void Context::provide(const vector<string_view>& export_keys)
{
    shared_ptr<Pair> keys_list;
    if (has(MODULE_KEY))
    {
        if (const auto existing_exports = get(MODULE_KEY); existing_exports && existing_exports->is_pair())
        {
            keys_list = existing_exports->as_pair();
        }
    }
    else
    {
        keys_list = Pair::EMPTY;
    }
    for (const auto export_key : std::ranges::reverse_view(export_keys))
    {
        const shared_ptr<Pair> new_pair = Pair::single(Expr::make_symbol(export_key));
        if (!keys_list->empty())
        {
            new_pair->set_cdr(Expr::make_pair(keys_list));
        }
        keys_list = new_pair;
    }

    assign(MODULE_KEY, Expr::make_pair(keys_list));
}

void Context::provide_all()
{
    const shared_ptr<Pair> keys_list = Pair::single(Expr::make_symbol(EXPORTS_ALL_KEY));
    assign(MODULE_KEY, Expr::make_pair(keys_list));
}

shared_ptr<const Context> Context::get_module_exports() const
{
    const auto export_keys_expr = get(MODULE_KEY);
    if (!export_keys_expr || !export_keys_expr->is_pair())
    {
        return nullptr;
    }
    const shared_ptr<Context> exports = new_context();
    for (const auto export_keys = export_keys_expr->as_pair(); const auto key : *export_keys)
    {
        if (!key) break;
        if (!key->is_symbol())
        {
            throw std::runtime_error("module exports: expected symbol keys");
        }
        auto name = key->as_symbol();
        if (name == EXPORTS_ALL_KEY)
        {
            return shared_from_this();
        }
        if (has(name))
        {
            exports->add(name, get(name));
        }
        else
        {
            throw std::runtime_error("module exports: no such binding: " + view_to_string(name));
        }
    }
    return exports;
}

void Context::import_all(const shared_ptr<const Context>& other)
{
    for (const auto& [name, value] : other->bindings)
    {
        add(name, value);
    }
}



// ----- Module class implementation -----

Module::Module(string name)
    : id(std::move(name)), exports(make_root_context())
{
}

[[nodiscard]] const string& Module::get_name() const { return id; }

shared_ptr<const Context> Module::get_exports() const { return exports; }

// Load and evaluate a Scheme source file in a fresh module context.
// The module name defaults to the file stem (basename without extension).
Module Module::load_from_file(const string& path)
{
    namespace fs = std::filesystem;

    // Read file
    std::ifstream in(path, std::ios::in | std::ios::binary);
    if (!in)
    {
        throw GlomError("module: cannot open file: " + path);
    }
    std::ostringstream ss;
    ss << in.rdbuf();
    if (in.bad())
    {
        throw GlomError("module: failed reading file: " + path);
    }

    // Derive module name from path stem
    string mod_name;
    try
    {
        fs::path p(path);
        mod_name = p.stem().string();
        if (mod_name.empty()) mod_name = "anonymous";
    }
    catch (...)
    {
        mod_name = "anonymous";
    }

    // Fresh context populated with root bindings
    auto mod_ctx = make_root_context();
    mod_ctx->init_module();
    // Parse & eval whole file in module context
    const auto exprs = parse(ss.str());
    ::eval(mod_ctx, exprs);

    // Construct module and attach evaluated context
    Module m(std::move(mod_name));
    m.exports = mod_ctx->get_module_exports();
    return m;
}
