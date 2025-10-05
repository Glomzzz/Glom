//
// Created by glom on 10/1/25.
//
#include <gtest/gtest.h>
#include <vector>
#include <memory>

#include "expr.h"
#include "context.h"
#include "parser.h"
#include "eval.h"

class SchemeMathTest : public ::testing::Test
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
        return ::eval(context, exprs);
    }

    void perform(const std::string& input) const
    {
        const auto exprs = parse(input);
        ::eval(context, exprs);
    }

    static shared_ptr<Expr> parse_and_get_first(const std::string& input)
    {
        const auto exprs = parse(input);
        if (exprs->empty()) return Expr::NOTHING;
        return exprs->car();
    }

    std::shared_ptr<Context> context;
};


// sqrt function tests
TEST_F(SchemeMathTest, SquareRoot)
{
    EXPECT_EQ(integer(2), eval("(sqrt 4)")->as_number_int());
    EXPECT_NEAR(1.414, eval("(sqrt 2)")->as_number_real(), 0.001);
    EXPECT_EQ(integer(3), eval("(sqrt 9)")->as_number_int());
    EXPECT_EQ(integer(0), eval("(sqrt 0)")->as_number_int());
    EXPECT_NEAR(1.732, eval("(sqrt 3)")->as_number_real(), 0.001);
}

// isqrt function tests (integer square root)
TEST_F(SchemeMathTest, IntegerSquareRoot)
{
    EXPECT_EQ(integer(2), eval("(isqrt 4)")->as_number_int());
    EXPECT_EQ(integer(1), eval("(isqrt 2)")->as_number_int());
    EXPECT_EQ(integer(3), eval("(isqrt 9)")->as_number_int());
    EXPECT_EQ(integer(0), eval("(isqrt 0)")->as_number_int());
    EXPECT_EQ(integer(1), eval("(isqrt 3)")->as_number_int());
    EXPECT_EQ(integer(10), eval("(isqrt 100)")->as_number_int());
    EXPECT_EQ(integer(10), eval("(isqrt 110)")->as_number_int());
}

// log function tests (natural logarithm)
TEST_F(SchemeMathTest, Logarithm)
{
    EXPECT_EQ(integer(0), eval("(log 1)")->as_number_int());
    EXPECT_NEAR(1.0, eval("(log 2.71828)")->as_number_real(), 0.001);
    EXPECT_NEAR(2.302, eval("(log 10)")->as_number_real(), 0.001);
    EXPECT_NEAR(0.693, eval("(log 2)")->as_number_real(), 0.001);
}

// sin function tests
TEST_F(SchemeMathTest, Sine)
{
    EXPECT_EQ(integer(0), eval("(sin 0)")->as_number_int());
    EXPECT_NEAR(1.0, eval("(sin 1.5708)")->as_number_real(), 0.001); // π/2
    EXPECT_NEAR(0.0, eval("(sin 3.14159)")->as_number_real(), 0.001); // π
    EXPECT_NEAR(-1.0, eval("(sin 4.71239)")->as_number_real(), 0.001); // 3π/2
    EXPECT_NEAR(0.5, eval("(sin 0.523599)")->as_number_real(), 0.001); // π/6
}

// cos function tests
TEST_F(SchemeMathTest, Cosine)
{
    EXPECT_EQ(integer(1), eval("(cos 0)")->as_number_int());
    EXPECT_NEAR(0.0, eval("(cos 1.5708)")->as_number_real(), 0.001); // π/2
    EXPECT_NEAR(-1.0, eval("(cos 3.14159)")->as_number_real(), 0.001); // π
    EXPECT_NEAR(0.0, eval("(cos 4.71239)")->as_number_real(), 0.001); // 3π/2
    EXPECT_NEAR(0.866, eval("(cos 0.523599)")->as_number_real(), 0.001); // π/6
}

// tan function tests
TEST_F(SchemeMathTest, Tangent)
{
    EXPECT_EQ(integer(0), eval("(tan 0)")->as_number_int());
    EXPECT_NEAR(1.0, eval("(tan 0.785398)")->as_number_real(), 0.001); // π/4
    EXPECT_NEAR(0.577, eval("(tan 0.523599)")->as_number_real(), 0.001); // π/6
    EXPECT_NEAR(1.732, eval("(tan 1.0472)")->as_number_real(), 0.001); // π/3
}

// asin function tests (arcsine)
TEST_F(SchemeMathTest, ArcSine)
{
    EXPECT_EQ(integer(0), eval("(asin 0)")->as_number_int());
    EXPECT_NEAR(1.5708, eval("(asin 1)")->as_number_real(), 0.001); // π/2
    EXPECT_NEAR(-1.5708, eval("(asin -1)")->as_number_real(), 0.001); // -π/2
    EXPECT_NEAR(0.523599, eval("(asin 0.5)")->as_number_real(), 0.001); // π/6
}

// acos function tests (arccosine)
TEST_F(SchemeMathTest, ArcCosine)
{
    EXPECT_NEAR(1.5708, eval("(acos 0)")->as_number_real(), 0.001); // π/2
    EXPECT_EQ(integer(0), eval("(acos 1)")->as_number_int());
    EXPECT_NEAR(3.14159, eval("(acos -1)")->as_number_real(), 0.001); // π
    EXPECT_NEAR(1.0472, eval("(acos 0.5)")->as_number_real(), 0.001); // π/3
}

// atan function tests (arctangent)
TEST_F(SchemeMathTest, ArcTangent)
{
    EXPECT_EQ(integer(0), eval("(atan 0)")->as_number_int());
    EXPECT_NEAR(0.785398, eval("(atan 1)")->as_number_real(), 0.001); // π/4
    EXPECT_NEAR(-0.785398, eval("(atan -1)")->as_number_real(), 0.001); // -π/4
    EXPECT_NEAR(0.463648, eval("(atan 0.5)")->as_number_real(), 0.001); // ~26.565°
}

// exp function tests (exponential)
TEST_F(SchemeMathTest, Exponential)
{
    EXPECT_EQ(integer(0), eval("(exp 0)")->as_number_int());
    EXPECT_NEAR(2.71828, eval("(exp 1)")->as_number_real(), 0.001); // e
    EXPECT_NEAR(7.38906, eval("(exp 2)")->as_number_real(), 0.001); // e^2
    EXPECT_NEAR(0.367879, eval("(exp -1)")->as_number_real(), 0.001); // 1/e
    EXPECT_NEAR(0.135335, eval("(exp -2)")->as_number_real(), 0.001); // 1/e^2
}

