//
// Created by glom on 10/1/25.
//
#include <gtest/gtest.h>
#include <vector>
#include <memory>

#include "expr.h"
#include "context.h"
#include "parser.h"

class SchemeNumComparatorsTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        context = make_root_context();
    }

    void TearDown() override
    {
        context.reset();
    }

    [[nodiscard]] shared_ptr<Expr> eval(const std::string& input) const
    {
        const auto exprs = parse(input);
        return context->eval(exprs);
    }

    void perform(const std::string& input) const
    {
        const auto exprs = parse(input);
        context->eval(exprs);
    }

    static shared_ptr<Expr> parse_and_get_first(const std::string& input)
    {
        auto exprs = parse(input);
        if (exprs.empty()) return Expr::NOTHING;
        return exprs[0];
    }

    std::shared_ptr<Context> context;
};


// Number comparison tests
TEST_F(SchemeNumComparatorsTest, NumericEquality)
{
    EXPECT_EQ(Expr::TRUE, eval("(= 1 1)"));
    EXPECT_EQ(Expr::TRUE, eval("(= 1.0 1)"));
    EXPECT_EQ(Expr::FALSE, eval("(= 1 2)"));
    EXPECT_EQ(Expr::TRUE, eval("(= 1 1 1)"));
    EXPECT_EQ(Expr::FALSE, eval("(= 1 1 2)"));
}

TEST_F(SchemeNumComparatorsTest, LessThan)
{
    EXPECT_EQ(Expr::TRUE, eval("(< 1 2)"));
    EXPECT_EQ(Expr::FALSE, eval("(< 2 1)"));
    EXPECT_EQ(Expr::FALSE, eval("(< 1 1)"));
    EXPECT_EQ(Expr::TRUE, eval("(< 1 2 3)"));
    EXPECT_EQ(Expr::FALSE, eval("(< 1 3 2)"));
}

TEST_F(SchemeNumComparatorsTest, GreaterThan)
{
    EXPECT_EQ(Expr::TRUE, eval("(> 2 1)"));
    EXPECT_EQ(Expr::FALSE, eval("(> 1 2)"));
    EXPECT_EQ(Expr::FALSE, eval("(> 1 1)"));
    EXPECT_EQ(Expr::TRUE, eval("(> 3 2 1)"));
    EXPECT_EQ(Expr::FALSE, eval("(> 3 1 2)"));
}

TEST_F(SchemeNumComparatorsTest, LessThanOrEqual)
{
    EXPECT_EQ(Expr::TRUE, eval("(<= 1 2)"));
    EXPECT_EQ(Expr::FALSE, eval("(<= 2 1)"));
    EXPECT_EQ(Expr::TRUE, eval("(<= 1 1)"));
    EXPECT_EQ(Expr::TRUE, eval("(<= 1 1 2)"));
    EXPECT_EQ(Expr::FALSE, eval("(<= 1 2 1)"));
}

TEST_F(SchemeNumComparatorsTest, GreaterThanOrEqual)
{
    EXPECT_EQ(Expr::TRUE, eval("(>= 2 1)"));
    EXPECT_EQ(Expr::FALSE, eval("(>= 1 2)"));
    EXPECT_EQ(Expr::TRUE, eval("(>= 1 1)"));
    EXPECT_EQ(Expr::TRUE, eval("(>= 3 2 1)"));
    EXPECT_EQ(Expr::FALSE, eval("(>= 1 2 1)"));
}