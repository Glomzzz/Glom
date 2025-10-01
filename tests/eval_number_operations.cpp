//
// Created by glom on 10/1/25.
//
#include <gtest/gtest.h>
#include <vector>
#include <memory>

#include "expr.h"
#include "context.h"
#include "parser.h"

class SchemeNumOperationsTest : public ::testing::Test
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

// Integer operations tests
TEST_F(SchemeNumOperationsTest, IntegerAddition)
{
    // Basic integer addition
    EXPECT_EQ(integer(0), eval("(+)")->as_number_int());
    EXPECT_EQ(integer(1), eval("(+ 1)")->as_number_int());
    EXPECT_EQ(integer(10), eval("(+ 1 2 3 4)")->as_number_int());
    EXPECT_EQ(integer(-5), eval("(+ 2 -7)")->as_number_int());
    EXPECT_EQ(integer(0), eval("(+ 5 -5)")->as_number_int());
    EXPECT_EQ(integer(1000000), eval("(+ 500000 500000)")->as_number_int());
}

TEST_F(SchemeNumOperationsTest, IntegerSubtraction)
{
    // Basic integer subtraction
    EXPECT_EQ(integer(0), eval("(- 0)")->as_number_int());
    EXPECT_EQ(integer(-5), eval("(- 5)")->as_number_int());
    EXPECT_EQ(integer(3), eval("(- 10 7)")->as_number_int());
    EXPECT_EQ(integer(-1), eval("(- 5 6)")->as_number_int());
    EXPECT_EQ(integer(5), eval("(- 10 3 2)")->as_number_int());
    EXPECT_EQ(integer(-10), eval("(- 0 5 5)")->as_number_int());
}

TEST_F(SchemeNumOperationsTest, IntegerMultiplication)
{
    // Basic integer multiplication
    EXPECT_EQ(integer(1), eval("(*)")->as_number_int());
    EXPECT_EQ(integer(5), eval("(* 5)")->as_number_int());
    EXPECT_EQ(integer(24), eval("(* 1 2 3 4)")->as_number_int());
    EXPECT_EQ(integer(-20), eval("(* 4 -5)")->as_number_int());
    EXPECT_EQ(integer(20), eval("(* -4 -5)")->as_number_int());
    EXPECT_EQ(integer(0), eval("(* 5 0 10)")->as_number_int());
}

TEST_F(SchemeNumOperationsTest, IntegerDivision)
{
    // Integer division
    EXPECT_EQ(integer(2), eval("(/ 10 5)")->as_number_int());
    EXPECT_EQ(rational(integer(1),integer(2)), eval("(/ 5 10)")->as_number_rat());
    EXPECT_EQ(integer(-2), eval("(/ -10 5)")->as_number_int());
    EXPECT_EQ(integer(-2), eval("(/ 10 -5)")->as_number_int());
    EXPECT_EQ(integer(2), eval("(/ -10 -5)")->as_number_int());
    EXPECT_EQ(integer(2), eval("(/ 20 5 2)")->as_number_int());
}

TEST_F(SchemeNumOperationsTest, IntegerExponentiation)
{
    // Integer exponentiation
    EXPECT_EQ(integer(1), eval("(expt 5 0)")->as_number_int());
    EXPECT_EQ(integer(5), eval("(expt 5 1)")->as_number_int());
    EXPECT_EQ(integer(25), eval("(expt 5 2)")->as_number_int());
    EXPECT_EQ(integer(1), eval("(expt 1 100)")->as_number_int());
    EXPECT_EQ(integer(0), eval("(expt 0 5)")->as_number_int());
}

TEST_F(SchemeNumOperationsTest, IntegerRemainder)
{
    // Integer remainder operation
    EXPECT_EQ(integer(1), eval("(remainder 10 3)")->as_number_int());
    EXPECT_EQ(integer(0), eval("(remainder 10 5)")->as_number_int());
    EXPECT_EQ(integer(2), eval("(remainder 10 4)")->as_number_int());
    EXPECT_EQ(integer(-1), eval("(remainder -10 3)")->as_number_int());
    EXPECT_EQ(integer(1), eval("(remainder 10 -3)")->as_number_int());
    EXPECT_EQ(integer(-1), eval("(remainder -10 -3)")->as_number_int());
}

TEST_F(SchemeNumOperationsTest, IntegerModulo)
{
    // Integer modulo operation
    EXPECT_EQ(integer(1), eval("(modulo 10 3)")->as_number_int());
    EXPECT_EQ(integer(0), eval("(modulo 10 5)")->as_number_int());
    EXPECT_EQ(integer(2), eval("(modulo 10 4)")->as_number_int());
    EXPECT_EQ(integer(1), eval("(modulo -10 3)")->as_number_int());
    EXPECT_EQ(integer(-1), eval("(modulo 10 -3)")->as_number_int());
    EXPECT_EQ(integer(-1), eval("(modulo -10 -3)")->as_number_int());
}

// Rational number operations tests
TEST_F(SchemeNumOperationsTest, RationalAddition)
{
    // Rational number addition
    EXPECT_EQ(rational(integer(3), integer(2)), eval("(+ 1/2 1)")->as_number_rat());
    EXPECT_EQ(rational(integer(5), integer(6)), eval("(+ 1/3 1/2)")->as_number_rat());
    EXPECT_EQ(rational(integer(11), integer(4)), eval("(+ 1/2 1/4 2)")->as_number_rat());
    EXPECT_EQ(integer(1), eval("(+ 1/2 1/2)")->as_number_int());
    EXPECT_EQ(rational(integer(-1), integer(6)), eval("(+ 1/3 -1/2)")->as_number_rat());
}

TEST_F(SchemeNumOperationsTest, RationalSubtraction)
{
    // Rational number subtraction
    EXPECT_EQ(rational(integer(-1), integer(2)), eval("(- 1/2 1)")->as_number_rat());
    EXPECT_EQ(rational(integer(-1), integer(6)), eval("(- 1/3 1/2)")->as_number_rat());
    EXPECT_EQ(rational(integer(1), integer(4)), eval("(- 1 3/4)")->as_number_rat());
    EXPECT_EQ(rational(integer(5), integer(6)), eval("(- 1/3 -1/2)")->as_number_rat());
}

TEST_F(SchemeNumOperationsTest, RationalMultiplication)
{
    // Rational number multiplication
    EXPECT_EQ(rational(integer(1), integer(6)), eval("(* 1/2 1/3)")->as_number_rat());
    EXPECT_EQ(rational(integer(3), integer(4)), eval("(* 3/2 1/2)")->as_number_rat());
    EXPECT_EQ(integer(1), eval("(* 2/3 3/2)")->as_number_int());
    EXPECT_EQ(rational(integer(-1), integer(4)), eval("(* 1/2 -1/2)")->as_number_rat());
    EXPECT_EQ(rational(integer(1), integer(4)), eval("(* -1/2 -1/2)")->as_number_rat());
}

TEST_F(SchemeNumOperationsTest, RationalDivision)
{
    // Rational number division
    EXPECT_EQ(rational(integer(3), integer(2)), eval("(/ 1/2 1/3)")->as_number_rat());
    EXPECT_EQ(rational(integer(1), integer(4)), eval("(/ 1/2 2)")->as_number_rat());
    EXPECT_EQ(integer(4), eval("(/ 2 1/2)")->as_number_int());
    EXPECT_EQ(rational(integer(-3), integer(2)), eval("(/ 1/2 -1/3)")->as_number_rat());
}

TEST_F(SchemeNumOperationsTest, RationalExponentiation)
{
    // Rational number exponentiation
    EXPECT_EQ(rational(integer(1), integer(4)), eval("(expt 1/2 2)")->as_number_rat());
    EXPECT_EQ(rational(integer(1), integer(8)), eval("(expt 1/2 3)")->as_number_rat());
    EXPECT_EQ(integer(8), eval("(expt 2 3)")->as_number_int());
    EXPECT_EQ(integer(1), eval("(expt 3/4 0)")->as_number_int());
}

// Real number operations tests
TEST_F(SchemeNumOperationsTest, RealAddition)
{
    // Real number addition with precision
    EXPECT_EQ(integer(0),eval("(+)")->as_number_int());
    EXPECT_DOUBLE_EQ(3.14159, eval("(+ 3.14159)")->as_number_real());
    std::cout << eval("(+ 1 2 3 0.9111)")->to_string() << std::endl;
    EXPECT_DOUBLE_EQ(6.9111, eval("(+ 1 2 3 0.9111)")->as_number_real());
    EXPECT_DOUBLE_EQ(10.5, eval("(+ 1.5 2.5 3.5 2.0 1.0)")->as_number_real());
    EXPECT_DOUBLE_EQ(-2.5, eval("(+ 1.5 -4.0)")->as_number_real());
}

TEST_F(SchemeNumOperationsTest, RealSubtraction)
{
    // Real number subtraction
    EXPECT_DOUBLE_EQ(-3.14, eval("(- 3.14)")->as_number_real());
    EXPECT_DOUBLE_EQ(1.5, eval("(- 5.5 4.0)")->as_number_real());
    EXPECT_DOUBLE_EQ(-3.0, eval("(- 2.0 5.0)")->as_number_real());
    EXPECT_DOUBLE_EQ(1.0, eval("(- 10.0 5.0 3.0 1.0)")->as_number_real());
    EXPECT_DOUBLE_EQ(-8.5, eval("(- 1.5 10.0)")->as_number_real());
}

TEST_F(SchemeNumOperationsTest, RealMultiplication)
{
    // Real number multiplication
    EXPECT_EQ(integer(1),eval("(*)")->as_number_int());
    EXPECT_DOUBLE_EQ(2.5, eval("(* 2.5)")->as_number_real());
    EXPECT_DOUBLE_EQ(30.0, eval("(* 2.0 3.0 5.0)")->as_number_real());
    EXPECT_DOUBLE_EQ(-12.0, eval("(* 3.0 -4.0)")->as_number_real());
    EXPECT_DOUBLE_EQ(12.0, eval("(* -3.0 -4.0)")->as_number_real());
    EXPECT_DOUBLE_EQ(0.0, eval("(* 5.0 0.0 10.0)")->as_number_real());
}

TEST_F(SchemeNumOperationsTest, RealDivision)
{
    // Real number division
    EXPECT_DOUBLE_EQ(0.5, eval("(/ 2.0)")->as_number_real());
    EXPECT_DOUBLE_EQ(2.0, eval("(/ 10.0 5.0)")->as_number_real());
    EXPECT_DOUBLE_EQ(0.4, eval("(/ 2.0 5.0)")->as_number_real());
    EXPECT_DOUBLE_EQ(-2.0, eval("(/ -10.0 5.0)")->as_number_real());
    EXPECT_DOUBLE_EQ(-2.0, eval("(/ 10.0 -5.0)")->as_number_real());
    EXPECT_DOUBLE_EQ(2.0, eval("(/ -10.0 -5.0)")->as_number_real());
    EXPECT_DOUBLE_EQ(2.0, eval("(/ 20.0 5.0 2.0)")->as_number_real());
}

TEST_F(SchemeNumOperationsTest, RealExponentiation)
{
    // Real number exponentiation
    EXPECT_DOUBLE_EQ(1.0, eval("(expt 5.0 0.0)")->as_number_real());
    EXPECT_DOUBLE_EQ(5.0, eval("(expt 5.0 1.0)")->as_number_real());
    EXPECT_DOUBLE_EQ(25.0, eval("(expt 5.0 2.0)")->as_number_real());
    EXPECT_DOUBLE_EQ(2.0, eval("(expt 4.0 0.5)")->as_number_real());
    EXPECT_DOUBLE_EQ(0.5, eval("(expt 4.0 -0.5)")->as_number_real());
}

// Mixed type operations tests
TEST_F(SchemeNumOperationsTest, MixedTypeAddition)
{
    // Mixed type addition with automatic promotion
    EXPECT_EQ(rational(integer(3), integer(2)), eval("(+ 1 1/2)")->as_number_rat());
    EXPECT_DOUBLE_EQ(3.5, eval("(+ 1 1/2 2.0)")->as_number_real());
    EXPECT_DOUBLE_EQ(4.14159, eval("(+ 1 1/2 2.0 0.64159)")->as_number_real());
    EXPECT_DOUBLE_EQ(2.5, eval("(+ 1/2 2.0)")->as_number_real());
}

TEST_F(SchemeNumOperationsTest, MixedTypeSubtraction)
{
    // Mixed type subtraction
    EXPECT_EQ(rational(integer(1), integer(2)), eval("(- 1 1/2)")->as_number_rat());
    EXPECT_EQ(rational(integer(1), integer(2)), eval("(- 1 1/2)")->as_number_rat());
    EXPECT_DOUBLE_EQ(-1.5, eval("(- 1/2 2.0)")->as_number_real());
    EXPECT_DOUBLE_EQ(1.5, eval("(- 2.0 1/2)")->as_number_real());
}

TEST_F(SchemeNumOperationsTest, MixedTypeMultiplication)
{
    // Mixed type multiplication
    EXPECT_EQ(rational(integer(3), integer(2)), eval("(* 3 1/2)")->as_number_rat());
    EXPECT_EQ(3.0, eval("(* 3 1.0 1/2 2.0)")->as_number_real());
    EXPECT_EQ(2.5, eval("(* 5 1/2 1.0)")->as_number_real());
}

TEST_F(SchemeNumOperationsTest, MixedTypeDivision)
{
    // Mixed type division
    EXPECT_EQ(integer(2), eval("(/ 1 1/2)")->as_number_int());
    EXPECT_EQ(rational(integer(1), integer(2)), eval("(/ 1/2 1)")->as_number_rat());
    EXPECT_EQ(integer(4), eval("(/ 2 1/2)")->as_number_int());
    EXPECT_EQ(2.0, eval("(/ 4.0 2)")->as_number_real());
}

// Complex expression tests with multiple operations
TEST_F(SchemeNumOperationsTest, ComplexExpressions)
{
    // Complex expressions with mixed operations
    EXPECT_EQ(integer(14), eval("(+ (* 3 4) (- 10 8))")->as_number_int());
    EXPECT_EQ(rational(integer(7), integer(2)), eval("(+ (/ 1 2) (* 3 1))")->as_number_rat());
    EXPECT_DOUBLE_EQ(8.5, eval("(+ (/ 1 2) (* 3.0 2.0) 2.0)")->as_number_real());
    EXPECT_EQ(integer(8), eval("(expt 2 3)")->as_number_int());
    EXPECT_DOUBLE_EQ(8.0, eval("(expt 2.0 3.0)")->as_number_real());
}

// Edge cases and error conditions
TEST_F(SchemeNumOperationsTest, EdgeCases)
{
    // Division by zero should throw
    EXPECT_THROW(perform("(/ 1 0)"), std::runtime_error);
    EXPECT_EQ(INFINITY, eval("(/ 1.0 0.0)")->as_number_real());

    // Remainder with zero divisor
    EXPECT_THROW(perform("(remainder 10 0)"), std::runtime_error);

    // Modulo with zero divisor
    EXPECT_THROW(perform("(modulo 10 0)"), std::runtime_error);

    // Large numbers
    EXPECT_EQ(integer(1000000000), eval("(+ 500000000 500000000)")->as_number_int());
    EXPECT_DOUBLE_EQ(1e18, eval("(* 1e9 1e9)")->as_number_real());
}

// Type conversion and simplification tests
TEST_F(SchemeNumOperationsTest, TypeConversion)
{
    // Integer to rational conversion
    EXPECT_EQ(integer(5), eval("(/ 10 2)")->as_number_int());

    // Rational to real conversion
    EXPECT_DOUBLE_EQ(0.5, eval("(+ 1/2 0.0)")->as_number_real());

    // Simplification tests
    EXPECT_EQ(integer(2), eval("(/ 4 2)")->as_number_int());
    EXPECT_EQ(integer(1), eval("(/ 3 3)")->as_number_int());
    EXPECT_EQ(rational(integer(1), integer(2)), eval("(/ 2 4)")->as_number_rat());
}

// Arbitrary precision integer tests
TEST_F(SchemeNumOperationsTest, ArbitraryPrecisionIntegers)
{
    // Large integer operations
    const auto large_num1 = eval("(+ 100000000000000000000 200000000000000000000)");
    const auto large_num2 = eval("(* 100000000000000000000 3)");

    // These should be equal
    EXPECT_TRUE(large_num1->as_number_int() == large_num2->as_number_int());

    // Very large exponentiation
    const auto large_power = eval("(expt 2 100)");
    EXPECT_TRUE(large_power->is_number_int());

    // Division of large integers producing rational
    const auto large_ratio = eval("(/ 100000000000000000000 3)");
    EXPECT_TRUE(large_ratio->is_number_rat());
}

// Additional comprehensive tests
TEST_F(SchemeNumOperationsTest, ComprehensiveOperations)
{
    // Test all operations together
    EXPECT_DOUBLE_EQ(27.0, eval("(+ (* 3 4) (- 10 2) (/ 9 2) 2.5)")->as_number_real());

    // Nested operations
    EXPECT_EQ(integer(26), eval("(+ (* 3 (+ 2 4)) (- 10 2))")->as_number_int());

    // Mixed with remainder and modulo
    EXPECT_EQ(integer(1), eval("(remainder (+ 10 5) 7)")->as_number_int());
    EXPECT_EQ(integer(1), eval("(modulo (- 10 5) 4)")->as_number_int());
}