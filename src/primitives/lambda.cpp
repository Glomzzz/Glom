//
// Created by glom on 9/27/25.
//

#include "error.h"
#include "context.h"
#include "expr.h"
#include "primitive.h"


shared_ptr<Expr> primitives::lambda(shared_ptr<Context> context, const shared_ptr<Pair>&& args)
{
    if (args->empty())
    {
        throw GlomError("Invalid number of arguments lambda: at least two arguments required");
    }
    const shared_ptr<Expr> params_expr = args->car();
    const auto body = args->cdr()->as_pair();
    if (body->empty())
    {
        throw GlomError("Invalid number of arguments lambda: at least two arguments required");
    }

    vector<Param> params;

    if (params_expr->is_symbol())
    {
        params.emplace_back(params_expr->as_symbol(), true);
    }
    else if (params_expr->is_pair())
    {
        const auto& param_list = params_expr->as_pair();

        bool variadic_found = false;
        bool variadic_end = false;
        for (const auto& param : *param_list)
        {
            if (!param) break;
            if (variadic_end)
            {
                throw GlomError("Invalid parameters in lambda: more than one item found after dot (.)");
            }
            if (!param->is_symbol())
            {
                throw GlomError("Invalid parameter in lambda: expected symbol, got " + param->to_string());
            }

            auto param_name = param->as_symbol();
            if (param_name == ".")
            {
                variadic_found = true;
                continue;
            }

            // Check if param_name is already in param_names
            for (const auto& existing_param : params)
            {
                if (existing_param.get_name() == param_name)
                {
                    throw GlomError("Duplicate parameter name in lambda: " + view_to_string(param_name));
                }
            }
            params.emplace_back(param_name, variadic_found);
            if (variadic_found)
            {
                variadic_end = true;
            }
        }
    }

    Lambda lambda(std::move(params), body, std::move(context));
    return Expr::make_lambda(std::make_shared<Lambda>(std::move(lambda)));
}
