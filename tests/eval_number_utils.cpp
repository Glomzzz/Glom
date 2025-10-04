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

class SchemeNumUtilsTest : public ::testing::Test
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


// zero? predicate tests
TEST_F(SchemeNumUtilsTest, IsZero)
{
    EXPECT_EQ(Expr::TRUE, eval("(zero? 0)"));
    EXPECT_EQ(Expr::TRUE, eval("(zero? 0.0)"));
    EXPECT_EQ(Expr::FALSE, eval("(zero? 1)"));
    EXPECT_EQ(Expr::FALSE, eval("(zero? -1)"));
    EXPECT_EQ(Expr::FALSE, eval("(zero? 0.1)"));
}

// positive? predicate tests
TEST_F(SchemeNumUtilsTest, IsPositive)
{
    EXPECT_EQ(Expr::TRUE, eval("(positive? 1)"));
    EXPECT_EQ(Expr::TRUE, eval("(positive? 1.5)"));
    EXPECT_EQ(Expr::TRUE, eval("(positive? 0.1)"));
    EXPECT_EQ(Expr::FALSE, eval("(positive? 0)"));
    EXPECT_EQ(Expr::FALSE, eval("(positive? -1)"));
    EXPECT_EQ(Expr::FALSE, eval("(positive? -1.5)"));
}

// negative? predicate tests
TEST_F(SchemeNumUtilsTest, IsNegative)
{
    EXPECT_EQ(Expr::TRUE, eval("(negative? -1)"));
    EXPECT_EQ(Expr::TRUE, eval("(negative? -1.5)"));
    EXPECT_EQ(Expr::TRUE, eval("(negative? -0.1)"));
    EXPECT_EQ(Expr::FALSE, eval("(negative? 0)"));
    EXPECT_EQ(Expr::FALSE, eval("(negative? 1)"));
    EXPECT_EQ(Expr::FALSE, eval("(negative? 1.5)"));
}

// even? predicate tests
TEST_F(SchemeNumUtilsTest, IsEven)
{
    EXPECT_EQ(Expr::TRUE, eval("(even? 0)"));
    EXPECT_EQ(Expr::TRUE, eval("(even? 2)"));
    EXPECT_EQ(Expr::TRUE, eval("(even? -2)"));
    EXPECT_EQ(Expr::TRUE, eval("(even? 4)"));
    EXPECT_EQ(Expr::FALSE, eval("(even? 1)"));
    EXPECT_EQ(Expr::FALSE, eval("(even? -1)"));
    EXPECT_EQ(Expr::FALSE, eval("(even? 3)"));
}

// odd? predicate tests
TEST_F(SchemeNumUtilsTest, IsOdd)
{
    EXPECT_EQ(Expr::TRUE, eval("(odd? 1)"));
    EXPECT_EQ(Expr::TRUE, eval("(odd? -1)"));
    EXPECT_EQ(Expr::TRUE, eval("(odd? 3)"));
    EXPECT_EQ(Expr::TRUE, eval("(odd? -3)"));
    EXPECT_EQ(Expr::FALSE, eval("(odd? 0)"));
    EXPECT_EQ(Expr::FALSE, eval("(odd? 2)"));
    EXPECT_EQ(Expr::FALSE, eval("(odd? -2)"));
}

// max function tests
TEST_F(SchemeNumUtilsTest, Max)
{
    EXPECT_EQ(integer(3), eval("(max 1 2 3)")->as_number_int());
    EXPECT_EQ(integer(5), eval("(max 5 2 1)")->as_number_int());
    EXPECT_EQ(3.5, eval("(max 1.5 3.5 2.0)")->as_number_real());
    EXPECT_EQ(integer(0), eval("(max -1 -2 0)")->as_number_int());
    EXPECT_EQ(integer(-1), eval("(max -1 -2 -3)")->as_number_int());
    EXPECT_EQ(integer(10), eval("(max 10)")->as_number_int());
    EXPECT_EQ(integer(3), eval("(max 3 3)")->as_number_int());
}

// min function tests
TEST_F(SchemeNumUtilsTest, Min)
{
    EXPECT_EQ(integer(1), eval("(min 1 2 3)")->as_number_int());
    EXPECT_EQ(integer(1), eval("(min 5 2 1)")->as_number_int());
    EXPECT_EQ(1.5, eval("(min 1.5 3.5 2.0)")->as_number_real());
    EXPECT_EQ(integer(-2), eval("(min -1 -2 0)")->as_number_int());
    EXPECT_EQ(integer(-3), eval("(min -1 -2 -3)")->as_number_int());
    EXPECT_EQ(integer(10), eval("(min 10)")->as_number_int());
    EXPECT_EQ(integer(3), eval("(min 3 3)")->as_number_int());
}

// abs function tests
TEST_F(SchemeNumUtilsTest, Abs)
{
    EXPECT_EQ(integer(5), eval("(abs 5)")->as_number_int());
    EXPECT_EQ(integer(5), eval("(abs -5)")->as_number_int());
    EXPECT_EQ(integer(0), eval("(abs 0)")->as_number_int());
    EXPECT_DOUBLE_EQ(3.14, eval("(abs 3.14)")->as_number_real());
    EXPECT_DOUBLE_EQ(3.14, eval("(abs -3.14)")->as_number_real());
    EXPECT_DOUBLE_EQ(2.5, eval("(abs 2.5)")->as_number_real());
}

// gcd function tests
TEST_F(SchemeNumUtilsTest, Gcd)
{
    EXPECT_EQ(integer(2), eval("(gcd 4 6)")->as_number_int());
    EXPECT_EQ(integer(1), eval("(gcd 3 5)")->as_number_int());
    EXPECT_EQ(integer(5), eval("(gcd 5 0)")->as_number_int());
    EXPECT_EQ(integer(5), eval("(gcd 0 5)")->as_number_int());
    EXPECT_EQ(integer(0), eval("(gcd 0 0)")->as_number_int());
    EXPECT_EQ(integer(1), eval("(gcd 1 1)")->as_number_int());
    EXPECT_EQ(integer(6), eval("(gcd 12 18 24)")->as_number_int());
    EXPECT_EQ(integer(2), eval("(gcd 2)")->as_number_int());
}

// lcm function tests
TEST_F(SchemeNumUtilsTest, Lcm)
{
    EXPECT_EQ(integer(12), eval("(lcm 4 6)")->as_number_int());
    EXPECT_EQ(integer(15), eval("(lcm 3 5)")->as_number_int());
    EXPECT_EQ(integer(0), eval("(lcm 5 0)")->as_number_int());
    EXPECT_EQ(integer(0), eval("(lcm 0 5)")->as_number_int());
    EXPECT_EQ(integer(0), eval("(lcm 0 0)")->as_number_int());
    EXPECT_EQ(integer(1), eval("(lcm 1 1)")->as_number_int());
    EXPECT_EQ(integer(72), eval("(lcm 12 18 24)")->as_number_int());
    EXPECT_EQ(integer(2), eval("(lcm 2)")->as_number_int());
}

// floor function tests
TEST_F(SchemeNumUtilsTest, Floor)
{
    EXPECT_DOUBLE_EQ(3.0, eval("(floor 3.7)")->as_number_real());
    EXPECT_DOUBLE_EQ(3.0, eval("(floor 3.2)")->as_number_real());
    EXPECT_DOUBLE_EQ(3.0, eval("(floor 3.0)")->as_number_real());
    EXPECT_DOUBLE_EQ(-4.0, eval("(floor -3.7)")->as_number_real());
    EXPECT_DOUBLE_EQ(-4.0, eval("(floor -3.2)")->as_number_real());
    EXPECT_DOUBLE_EQ(0.0, eval("(floor 0.5)")->as_number_real());
    EXPECT_DOUBLE_EQ(-1.0, eval("(floor -0.5)")->as_number_real());
}

// ceiling function tests
TEST_F(SchemeNumUtilsTest, Ceiling)
{
    EXPECT_DOUBLE_EQ(4.0, eval("(ceiling 3.7)")->as_number_real());
    EXPECT_DOUBLE_EQ(4.0, eval("(ceiling 3.2)")->as_number_real());
    EXPECT_DOUBLE_EQ(3.0, eval("(ceiling 3.0)")->as_number_real());
    EXPECT_DOUBLE_EQ(-3.0, eval("(ceiling -3.7)")->as_number_real());
    EXPECT_DOUBLE_EQ(-3.0, eval("(ceiling -3.2)")->as_number_real());
    EXPECT_DOUBLE_EQ(1.0, eval("(ceiling 0.5)")->as_number_real());
    EXPECT_DOUBLE_EQ(0.0, eval("(ceiling -0.5)")->as_number_real());
}

// truncate function tests
TEST_F(SchemeNumUtilsTest, Truncate)
{
    EXPECT_DOUBLE_EQ(3.0, eval("(truncate 3.7)")->as_number_real());
    EXPECT_DOUBLE_EQ(3.0, eval("(truncate 3.2)")->as_number_real());
    EXPECT_DOUBLE_EQ(3.0, eval("(truncate 3.0)")->as_number_real());
    EXPECT_DOUBLE_EQ(-3.0, eval("(truncate -3.7)")->as_number_real());
    EXPECT_DOUBLE_EQ(-3.0, eval("(truncate -3.2)")->as_number_real());
    EXPECT_DOUBLE_EQ(0.0, eval("(truncate 0.5)")->as_number_real());
    EXPECT_DOUBLE_EQ(0.0, eval("(truncate -0.5)")->as_number_real());
}

// round function tests
TEST_F(SchemeNumUtilsTest, Round)
{
    EXPECT_DOUBLE_EQ(4.0, eval("(round 3.7)")->as_number_real());
    EXPECT_DOUBLE_EQ(3.0, eval("(round 3.2)")->as_number_real());
    EXPECT_DOUBLE_EQ(3.0, eval("(round 3.0)")->as_number_real());
    EXPECT_DOUBLE_EQ(4.0, eval("(round 3.5)")->as_number_real());
    EXPECT_DOUBLE_EQ(-4.0, eval("(round -3.7)")->as_number_real());
    EXPECT_DOUBLE_EQ(-3.0, eval("(round -3.2)")->as_number_real());
    EXPECT_DOUBLE_EQ(1.0, eval("(round 0.5)")->as_number_real());
    EXPECT_DOUBLE_EQ(-1.0, eval("(round -0.5)")->as_number_real());
}