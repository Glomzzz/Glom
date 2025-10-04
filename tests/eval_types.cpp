//
// Created by glom on 10/1/25.
//
#include <gtest/gtest.h>
#include <memory>

#include "expr.h"
#include "context.h"
#include "parser.h"
#include "eval.h"

class SchemeTypesTest : public ::testing::Test
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


// pair? predicate tests
TEST_F(SchemeTypesTest, IsPair)
{
    EXPECT_EQ(Expr::TRUE, eval("(pair? '(1 2))"));
    EXPECT_EQ(Expr::TRUE, eval("(pair? '(1 . 2))"));
    EXPECT_EQ(Expr::TRUE, eval("(pair? '(a b c))"));
    EXPECT_EQ(Expr::TRUE, eval("(pair? '())"));
    EXPECT_EQ(Expr::FALSE, eval("(pair? 5)"));
    EXPECT_EQ(Expr::FALSE, eval("(pair? #t)"));
    EXPECT_EQ(Expr::FALSE, eval("(pair? \"hello\")"));
    EXPECT_EQ(Expr::FALSE, eval("(pair? 'a)"));
}

// number? predicate tests
TEST_F(SchemeTypesTest, IsNumber)
{
    EXPECT_EQ(Expr::TRUE, eval("(number? 5)"));
    EXPECT_EQ(Expr::TRUE, eval("(number? 3.14)"));
    EXPECT_EQ(Expr::TRUE, eval("(number? -10)"));
    EXPECT_EQ(Expr::TRUE, eval("(number? 0)"));
    EXPECT_EQ(Expr::TRUE, eval("(number? 2/3)"));
    EXPECT_EQ(Expr::FALSE, eval("(number? #t)"));
    EXPECT_EQ(Expr::FALSE, eval("(number? 'a)"));
    EXPECT_EQ(Expr::FALSE, eval("(number? \"123\")"));
    EXPECT_EQ(Expr::FALSE, eval("(number? '(1 2 3))"));
}

// boolean? predicate tests
TEST_F(SchemeTypesTest, IsBoolean)
{
    EXPECT_EQ(Expr::TRUE, eval("(boolean? #t)"));
    EXPECT_EQ(Expr::TRUE, eval("(boolean? #f)"));
    EXPECT_EQ(Expr::FALSE, eval("(boolean? 0)"));
    EXPECT_EQ(Expr::FALSE, eval("(boolean? 1)"));
    EXPECT_EQ(Expr::FALSE, eval("(boolean? '())"));
    EXPECT_EQ(Expr::FALSE, eval("(boolean? '(1 2))"));
    EXPECT_EQ(Expr::FALSE, eval("(boolean? \"true\")"));
    EXPECT_EQ(Expr::FALSE, eval("(boolean? 'a)"));
}

// symbol? predicate tests
TEST_F(SchemeTypesTest, IsSymbol)
{
    EXPECT_EQ(Expr::TRUE, eval("(symbol? 'a)"));
    EXPECT_EQ(Expr::TRUE, eval("(symbol? 'hello)"));
    EXPECT_EQ(Expr::TRUE, eval("(symbol? '+)"));
    EXPECT_EQ(Expr::FALSE, eval("(symbol? \"a\")"));
    EXPECT_EQ(Expr::FALSE, eval("(symbol? 5)"));
    EXPECT_EQ(Expr::FALSE, eval("(symbol? #t)"));
    EXPECT_EQ(Expr::FALSE, eval("(symbol? '(a b))"));
    EXPECT_EQ(Expr::FALSE, eval("(symbol? '())"));
}

// string? predicate tests
TEST_F(SchemeTypesTest, IsString)
{
    EXPECT_EQ(Expr::TRUE, eval("(string? \"hello\")"));
    EXPECT_EQ(Expr::TRUE, eval("(string? \"\")"));
    EXPECT_EQ(Expr::TRUE, eval("(string? \"123\")"));
    EXPECT_EQ(Expr::FALSE, eval("(string? 'hello)"));
    EXPECT_EQ(Expr::FALSE, eval("(string? 123)"));
    EXPECT_EQ(Expr::FALSE, eval("(string? #t)"));
    EXPECT_EQ(Expr::FALSE, eval("(string? '(1 2))"));
    EXPECT_EQ(Expr::FALSE, eval("(string? '())"));
}

// exact? predicate tests
TEST_F(SchemeTypesTest, IsExact)
{
    EXPECT_EQ(Expr::TRUE, eval("(exact? 5)"));
    EXPECT_EQ(Expr::TRUE, eval("(exact? 2/3)"));
    EXPECT_EQ(Expr::TRUE, eval("(exact? -10)"));
    EXPECT_EQ(Expr::FALSE, eval("(exact? 3.14)"));
    EXPECT_EQ(Expr::FALSE, eval("(exact? 1.0)"));
}

// inexact? predicate tests
TEST_F(SchemeTypesTest, IsInexact)
{
    EXPECT_EQ(Expr::TRUE, eval("(inexact? 3.14)"));
    EXPECT_EQ(Expr::TRUE, eval("(inexact? 1.0)"));
    EXPECT_EQ(Expr::FALSE, eval("(inexact? 5)"));
    EXPECT_EQ(Expr::FALSE, eval("(inexact? 2/3)"));
    EXPECT_EQ(Expr::FALSE, eval("(inexact? -10)"));
}

// exact->inexact conversion tests
TEST_F(SchemeTypesTest, ExactToInexact)
{
    EXPECT_NEAR(5.0, eval("(exact->inexact 5)")->as_number_real(), 0.001);
    EXPECT_NEAR(-10.0, eval("(exact->inexact -10)")->as_number_real(), 0.001);
    EXPECT_NEAR(0.666, eval("(exact->inexact 2/3)")->as_number_real(), 0.001);
    EXPECT_NEAR(3.14, eval("(exact->inexact 3.14)")->as_number_real(), 0.001); // already inexact
}

// inexact->exact conversion tests
TEST_F(SchemeTypesTest, InexactToExact)
{
    EXPECT_EQ(integer(5), eval("(inexact->exact 5.0)")->as_number_int());
    EXPECT_EQ(integer(-10), eval("(inexact->exact -10.0)")->as_number_int());
    EXPECT_NEAR(3.14, eval("(inexact->exact 3.14)")->as_number_rat().to_inexact(), 0.001);
    EXPECT_NEAR(3.9, eval("(inexact->exact 3.9)")->as_number_rat().to_inexact(), 0.001);
}

// number->string conversion tests
TEST_F(SchemeTypesTest, NumberToString)
{
    // Test integer conversion
    const auto result1 = eval("(number->string 123)");
    EXPECT_TRUE(result1->is_string());
    EXPECT_EQ("123", result1->as_string());

    // Test float conversion
    const auto result2 = eval("(number->string 3.14)");
    EXPECT_TRUE(result2->is_string());

    // Test negative number
    const auto result3 = eval("(number->string -42)");
    EXPECT_TRUE(result3->is_string());
    EXPECT_EQ("-42", result3->as_string());
}

// string->number conversion tests
TEST_F(SchemeTypesTest, StringToNumber)
{
    // Test integer conversion
    EXPECT_EQ(integer(123), eval("(string->number \"123\")")->as_number_int());

    // Test float conversion
    EXPECT_NEAR(3.14, eval("(string->number \"3.14\")")->as_number_real(), 0.001);

    // Test negative number
    EXPECT_EQ(integer(-42), eval("(string->number \"-42\")")->as_number_int());

    // Test invalid string
    EXPECT_EQ(Expr::FALSE, eval("(string->number \"hello\")"));
    EXPECT_EQ(Expr::FALSE, eval("(string->number \"12a3\")"));
}

// symbol->string
TEST_F(SchemeTypesTest, SymbolToString)
{
    EXPECT_EQ("hello", eval("(symbol->string 'hello)")->as_string());
    EXPECT_EQ("+", eval("(symbol->string '+)")->as_string());
    EXPECT_EQ("a-b_c", eval("(symbol->string 'a-b_c)")->as_string());
}

// string->symbol
TEST_F(SchemeTypesTest, StringToSymbol)
{
    EXPECT_EQ("hello", eval("(string->symbol \"hello\")")->as_symbol());
    EXPECT_EQ("+", eval("(string->symbol \"+\")")->as_symbol());
}

// string=?
TEST_F(SchemeTypesTest, EqString)
{
    EXPECT_EQ(Expr::TRUE, eval("(string=? \"hello\" \"hello\")"));
    EXPECT_EQ(Expr::FALSE, eval("(string=? \"hello\" \"Hello\")"));
    EXPECT_EQ(Expr::TRUE, eval("(string=? \"\" \"\")"));
    EXPECT_EQ(Expr::FALSE, eval("(string=? \"abc\" \"abcd\")"));
    EXPECT_EQ(Expr::TRUE, eval("(string=? \"test\" \"test\" \"test\")"));
    EXPECT_EQ(Expr::FALSE, eval("(string=? \"test\" \"test\" \"Test\")"));
}

// string-ci=?
TEST_F(SchemeTypesTest, EqStringIgnoreCase)
{
    EXPECT_EQ(Expr::TRUE, eval("(string-ci=? \"hello\" \"Hello\")"));
    EXPECT_EQ(Expr::TRUE, eval("(string-ci=? \"Test\" \"test\")"));
    EXPECT_EQ(Expr::FALSE, eval("(string-ci=? \"abc\" \"abcd\")"));
    EXPECT_EQ(Expr::TRUE, eval("(string-ci=? \"test\" \"TEST\" \"TeSt\")"));
    EXPECT_EQ(Expr::FALSE, eval("(string-ci=? \"test\" \"test\" \"TesTt\")"));
}