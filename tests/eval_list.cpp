#include <gtest/gtest.h>
#include <vector>
#include <memory>

#include "expr.h"
#include "context.h"
#include "parser.h"
#include "eval.h"

class SchemeListTest : public ::testing::Test
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

// List operations tests
TEST_F(SchemeListTest, Cons)
{
    const auto result = eval("(cons 1 2)");
    EXPECT_EQ(PAIR, result->get_type());
    const auto pair = result->as_pair();
    EXPECT_EQ(integer(1), pair->car()->as_number_int());
    EXPECT_EQ(integer(2), pair->cdr()->as_number_int());

    // Test building lists
    const auto list = eval("(cons 1 (cons 2 '()))");
    EXPECT_EQ(PAIR, list->get_type());
}

TEST_F(SchemeListTest, Car)
{
    EXPECT_EQ(integer(1), eval("(car '(1 2 3))")->as_number_int());
    EXPECT_EQ(integer(1), eval("(car (cons 1 2))")->as_number_int());
}

TEST_F(SchemeListTest, Cdr)
{
    const auto result = eval("(cdr '(1 2 3))");
    EXPECT_EQ(PAIR, result->get_type());
    EXPECT_EQ(integer(2), result->as_pair()->car()->as_number_int());

    EXPECT_EQ(integer(2), eval("(cdr (cons 1 2))")->as_number_int());
}

TEST_F(SchemeListTest, List)
{
    const auto result = eval("(list 1 2 3)");
    EXPECT_EQ(PAIR, result->get_type());

    const auto pair = result->as_pair();
    EXPECT_EQ(integer(1),pair->car()->as_number_int());

    const auto second = pair->cdr()->as_pair();
    EXPECT_EQ(integer(2),second->car()->as_number_int());

    const auto third = second->cdr()->as_pair();
    EXPECT_EQ(integer(3),third->car()->as_number_int());
    EXPECT_TRUE(third->cdr()->is_nil());

    // Empty list
    EXPECT_TRUE(eval("(list)")->is_nil());
}

TEST_F(SchemeListTest, Append)
{
    const auto result = eval("(append '(1 2) '(3 4))");
    EXPECT_EQ(PAIR, result->get_type());

    auto pair = result->as_pair();
    EXPECT_EQ(integer(1),pair->car()->as_number_int());

    pair = pair->cdr()->as_pair();
    EXPECT_EQ(integer(2),pair->car()->as_number_int());

    pair = pair->cdr()->as_pair();
    EXPECT_EQ(integer(3),pair->car()->as_number_int());

    pair = pair->cdr()->as_pair();
    EXPECT_EQ(integer(4),pair->car()->as_number_int());
    EXPECT_TRUE(pair->cdr()->is_nil());

    // Append with empty list
    EXPECT_TRUE(eval("(append '() '(1 2))")->as_pair()->to_string() == "(1 2)");
    EXPECT_TRUE(eval("(append '(1 2) '())")->as_pair()->to_string() == "(1 2)");
    EXPECT_TRUE(eval("(append '() '())")->is_nil());
}

TEST_F(SchemeListTest, IsNull)
{
    EXPECT_EQ(Expr::TRUE, eval("(null? '())"));
    EXPECT_EQ(Expr::FALSE, eval("(null? '(1 2 3))"));
    EXPECT_EQ(Expr::FALSE, eval("(null? 1)"));
    EXPECT_EQ(Expr::FALSE, eval("(null? #t)"));
}

TEST_F(SchemeListTest, CarCdrCombinations)
{
    // caar - (car (car x))
    EXPECT_EQ(integer(1), eval("(caar '((1 2) 3 4))")->as_number_int());

    // cadr - (car (cdr x))
    EXPECT_EQ(integer(2), eval("(cadr '(1 2 3))")->as_number_int());

    // cdar - (cdr (car x))
    const auto cdar_result = eval("(cdar '((1 2 3) 4 5))");
    EXPECT_EQ(PAIR, cdar_result->get_type());
    EXPECT_EQ("(2 3)", cdar_result->to_string());

    // cddr - (cdr (cdr x))
    const auto cddr_result = eval("(cddr '(1 2 3 4))");
    EXPECT_EQ(PAIR, cddr_result->get_type());
    EXPECT_EQ("(3 4)", cddr_result->to_string());
}

TEST_F(SchemeListTest, ThreeLevelCombinations)
{
    // caaar - (car (car (car x)))
    EXPECT_EQ(integer(1), eval("(caaar '(((1 2) 3) 4))")->as_number_int());

    // caadr - (car (car (cdr x)))
    EXPECT_EQ(integer(2), eval("(caadr '(1 (2 (3 4)) 5))")->as_number_int());

    // cadar - (car (cdr (car x)))
    EXPECT_EQ(integer(2), eval("(cadar '((1 2 3) 4))")->as_number_int());

    // caddr - (car (cdr (cdr x)))
    EXPECT_EQ(integer(3), eval("(caddr '(1 2 3 4))")->as_number_int());

    // cdaar - (cdr (car (car x)))
    const auto cdaar_result = eval("(cdaar '(((1 2 3) 4) 5))");
    EXPECT_EQ(PAIR, cdaar_result->get_type());
    EXPECT_EQ("(2 3)", cdaar_result->to_string());

    // cdadr - (cdr (car (cdr x)))
    const auto cdadr_result = eval("(cdadr '(1 (2 3 4) 5))");
    EXPECT_EQ(PAIR, cdadr_result->get_type());
    EXPECT_EQ("(3 4)", cdadr_result->to_string());

    // cddar - (cdr (cdr (car x)))
    const auto cddar_result = eval("(cddar '((1 2 3 4) 5))");
    EXPECT_EQ(PAIR, cddar_result->get_type());
    EXPECT_EQ("(3 4)", cddar_result->to_string());

    // cdddr - (cdr (cdr (cdr x)))
    const auto cdddr_result = eval("(cdddr '(1 2 3 4 5))");
    EXPECT_EQ(PAIR, cdddr_result->get_type());
    EXPECT_EQ("(4 5)", cdddr_result->to_string());
}

TEST_F(SchemeListTest, FourLevelCombinations)
{
    // caaaar = (car (car (car (car x))))
    EXPECT_EQ(integer(1), eval("(caaaar '((((1 2) 3) 4) 5))")->as_number_int());

    // caaadr = (car (car (car (cdr x))))
    EXPECT_EQ(integer(4), eval("(caaadr '(5 ((4 3) 2) 1))")->as_number_int());

    // caadar = (car (car (cdr (car x))))
    EXPECT_EQ(integer(3), eval("(caadar '((2 (3) 6)))")->as_number_int());

    // caaddr = (car (car (cdr (cdr x))))
    EXPECT_EQ(integer(3), eval("(caaddr '(1 2 (3 4) 5 6))")->as_number_int());

    // cadaar = (car (cdr (car (car x))))
    EXPECT_EQ(integer(2), eval("(cadaar '(((1 2 3) 4) 5))")->as_number_int());

    // cadadr = (car (cdr (car (cdr x))))
    EXPECT_EQ(integer(3), eval("(cadadr '(1 (2 3 4) 5))")->as_number_int());

    // caddar = (car (cdr (cdr (car x))))
    EXPECT_EQ(integer(3), eval("(caddar '((1 2 3 4) 5))")->as_number_int());

    // cadddr = (car (cdr (cdr (cdr x))))
    EXPECT_EQ(integer(4), eval("(cadddr '(1 2 3 4 5))")->as_number_int());

    // cdaaar = (cdr (car (car (car x))))
    const auto cdaaar_result = eval("(cdaaar '((((1 2 3) 4) 5) 6))");
    EXPECT_EQ(PAIR, cdaaar_result->get_type());
    EXPECT_EQ("(2 3)", cdaaar_result->to_string());

    // cdaadr = (cdr (car (car (cdr x))))
    const auto cdaadr_result = eval("(cdaadr '(1 ((2 (3 4 5)) 6) 7))");
    EXPECT_EQ(PAIR, cdaadr_result->get_type());
    EXPECT_EQ("((3 4 5))", cdaadr_result->to_string());

    // cdadar = (cdr (car (cdr (car x))))
    const auto cdadar_result = eval("(cdadar '((1 (2 3 4) 5) 6))");
    EXPECT_EQ(PAIR, cdadar_result->get_type());
    EXPECT_EQ("(3 4)", cdadar_result->to_string());

    // cdaddr = (cdr (car (cdr (cdr x))))
    const auto cdaddr_result = eval("(cdaddr '(1 2 ((3 4) 5 6) 7))");
    EXPECT_EQ(PAIR, cdaddr_result->get_type());
    EXPECT_EQ("(5 6)", cdaddr_result->to_string());

    // cddaar = (cdr (cdr (car (car x))))
    const auto cddaar_result = eval("(cddaar '(((1 2 3 4) 5) 6))");
    EXPECT_EQ(PAIR, cddaar_result->get_type());
    EXPECT_EQ("(3 4)", cddaar_result->to_string());

    // cddadr = (cdr (cdr (car (cdr x))))
    const auto cddadr_result = eval("(cddadr '(1 (2 3 4 5) 6))");
    EXPECT_EQ(PAIR, cddadr_result->get_type());
    EXPECT_EQ("(4 5)", cddadr_result->to_string());

    // cdddar = (cdr (cdr (cdr (car x))))
    const auto cdddar_result = eval("(cdddar '((1 2 3 4 5) 6))");
    EXPECT_EQ(PAIR, cdddar_result->get_type());
    EXPECT_EQ("(4 5)", cdddar_result->to_string());

    // cddddr = (cdr (cdr (cdr (cdr x))))
    const auto cddddr_result = eval("(cddddr '(1 2 3 4 5 6))");
    EXPECT_EQ(PAIR, cddddr_result->get_type());
    EXPECT_EQ("(5 6)", cddddr_result->to_string());
}


TEST_F(SchemeListTest, CarCdrErrorCases)
{
    // Test error cases - operations on non-pairs
    EXPECT_THROW(perform("(car 1)"), std::runtime_error);
    EXPECT_THROW(perform("(cdr 1)"), std::runtime_error);
    EXPECT_THROW(perform("(caar 1)"), std::runtime_error);
    EXPECT_THROW(perform("(cadr 1)"), std::runtime_error);

    // Test error cases - operations on empty lists
    EXPECT_THROW(perform("(car '())"), std::runtime_error);
    EXPECT_THROW(perform("(cdr '())"), std::runtime_error);
    EXPECT_THROW(perform("(caar '())"), std::runtime_error);

    // Test error cases - insufficient nesting
    EXPECT_THROW(perform("(caar '(1))"), std::runtime_error);
    EXPECT_THROW(perform("(cadr '(1))"), std::runtime_error);
    EXPECT_THROW(perform("(caddr '(1 2))"), std::runtime_error);
}

TEST_F(SchemeListTest, AllCadrTest)
{
    // Level 1
    EXPECT_EQ("car", eval("(car '(car . cdr))")->to_string());
    EXPECT_EQ("cdr", eval("(cdr '(car . cdr))")->to_string());

    // Level 2
    EXPECT_EQ("caar", eval("(caar '((caar . cadr) . (cdar . cddr)))")->to_string());
    EXPECT_EQ("cadr", eval("(cadr '((caar . cdar) . (cadr . cddr)))")->to_string());
    EXPECT_EQ("cdar", eval("(cdar '((caar . cdar) . (cadr . cddr)))")->to_string());
    EXPECT_EQ("cddr", eval("(cddr '((caar . cdar) . (cadr . cddr)))")->to_string());

    // Level 3
    perform("(define list3 '(((caaar . cdaar) . (cadar . cddar)) . ((caadr . cdadr) . (caddr . cdddr))))");

    EXPECT_EQ("caaar", eval("(caaar list3)")->to_string());
    EXPECT_EQ("caadr", eval("(caadr list3)")->to_string());
    EXPECT_EQ("cadar", eval("(cadar list3)")->to_string());
    EXPECT_EQ("caddr", eval("(caddr list3)")->to_string());

    EXPECT_EQ("cdaar", eval("(cdaar list3)")->to_string());
    EXPECT_EQ("cdadr", eval("(cdadr list3)")->to_string());
    EXPECT_EQ("cddar", eval("(cddar list3)")->to_string());
    EXPECT_EQ("cdddr", eval("(cdddr list3)")->to_string());

    // Level 4
    perform(R"((define list4 '(
                (( (caaaar . cdaaar) . (cadaar . cddaar) ) . ( (caadar . cdadar) . (caddar . cdddar) )) .
                (( (cdaaar . cdaadr) . (cadadr . cddadr) ) . ( (caaddr . cdaddr) . (cadddr . cddddr) ))
)
))");

    EXPECT_EQ("caaaar", eval("(caaaar list4)")->to_string());
    EXPECT_EQ("cdaaar", eval("(cdaaar list4)")->to_string());
    EXPECT_EQ("caadar", eval("(caadar list4)")->to_string());
    EXPECT_EQ("caaddr", eval("(caaddr list4)")->to_string());
    EXPECT_EQ("cadaar", eval("(cadaar list4)")->to_string());
    EXPECT_EQ("cadadr", eval("(cadadr list4)")->to_string());
    EXPECT_EQ("caddar", eval("(caddar list4)")->to_string());
    EXPECT_EQ("cadddr", eval("(cadddr list4)")->to_string());
    EXPECT_EQ("cdaaar", eval("(cdaaar list4)")->to_string());
    EXPECT_EQ("cdaadr", eval("(cdaadr list4)")->to_string());
    EXPECT_EQ("cdadar", eval("(cdadar list4)")->to_string());
    EXPECT_EQ("cdaddr", eval("(cdaddr list4)")->to_string());
    EXPECT_EQ("cddaar", eval("(cddaar list4)")->to_string());
    EXPECT_EQ("cddadr", eval("(cddadr list4)")->to_string());
    EXPECT_EQ("cdddar", eval("(cdddar list4)")->to_string());
    EXPECT_EQ("cddddr", eval("(cddddr list4)")->to_string());
}