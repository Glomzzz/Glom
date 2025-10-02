#include <gtest/gtest.h>
#include "tokenizer.h"
#include "parser.h"
#include "expr.h"

class ParserTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(ParserTest, AtomParsing)
{
    auto exprs = parse("123 12312312312321312312312312412412412412412412412412 123/124 1748297128947981274891274.0 \"hello\" true abc");
    auto current = exprs->car();
    exprs = exprs->cdr()->as_pair();

    EXPECT_EQ(NUMBER_INT, current->get_type());
    EXPECT_EQ(integer(123), current->as_number_int());

    current = exprs->car();
    exprs = exprs->cdr()->as_pair();
    EXPECT_EQ(NUMBER_INT,current->get_type());
    EXPECT_EQ(integer("12312312312321312312312312412412412412412412412412"), current->as_number_int());


    current = exprs->car();
    exprs = exprs->cdr()->as_pair();
    EXPECT_EQ(NUMBER_RAT, current->get_type());
    EXPECT_EQ(rational(integer(123),integer(124)), current->as_number_rat());

    current = exprs->car();
    exprs = exprs->cdr()->as_pair();
    EXPECT_EQ(NUMBER_REAL, current->get_type());
    EXPECT_EQ(from_string("1748297128947981274891274.0"), current->as_number_real());

    current = exprs->car();
    exprs = exprs->cdr()->as_pair();
    EXPECT_EQ(STRING, current->get_type());
    EXPECT_EQ("hello", current->as_string());

    current = exprs->car();
    exprs = exprs->cdr()->as_pair();
    EXPECT_EQ(BOOLEAN, current->get_type());
    EXPECT_EQ(true, current->as_boolean());

    current = exprs->car();
    exprs = exprs->cdr()->as_pair();
    EXPECT_EQ(SYMBOL, current->get_type());
    EXPECT_EQ("abc", current->as_symbol());

    if (!exprs->empty())
        FAIL() << "Expected only 7 expressions, got more.";
}

TEST_F(ParserTest, ListParsing)
{
    auto exprs = parse("(1 2 3)");
    const shared_ptr<Expr> list = std::move(exprs->car());
    EXPECT_EQ(PAIR, list->get_type());
    int index = 0;
    for (const auto elem : *list->as_pair())
    {
        if (!elem) break;
        EXPECT_EQ(++index, elem->as_number_int().as_int64());
    }
}

TEST_F(ParserTest, QuoteParsing)
{
    auto exprs = parse("'(1 2 3)");
    const shared_ptr<Expr> list = std::move(exprs->car());
    EXPECT_EQ(PAIR, list->get_type());
    const auto& quote = list->as_pair();
    EXPECT_EQ("quote", quote->car()->as_symbol());
    const auto& elements = quote->cdr()->as_pair()->car()->as_pair();
    int index = 0;
    for (const auto elem : *elements)
    {
        if (!elem) break;
        EXPECT_EQ(++index, elem->as_number_int().as_int64());
    }
}

TEST_F(ParserTest, NilParsing)
{
    auto exprs = parse("'()");
    const shared_ptr<Expr> quoted_nil = std::move(exprs->car());
    EXPECT_EQ(PAIR, quoted_nil->get_type());
    const auto& quote = quoted_nil->as_pair();
    EXPECT_EQ("quote", quote->car()->as_symbol());
    const auto& nil_nil = quote->cdr()->as_pair();
    EXPECT_EQ(Expr::NIL, nil_nil->car());
    EXPECT_EQ(Expr::NIL, nil_nil->cdr());
}


TEST_F(ParserTest, NestedList)
{
    auto exprs = parse("((1 2) (3 4))");
    const shared_ptr<Expr> list = std::move(exprs->car());
    EXPECT_EQ(PAIR, list->get_type());
    const auto& elements = list->as_pair();
    EXPECT_EQ(PAIR, elements->car()->get_type());
    const auto& sublist1 = elements->car()->as_pair();
    EXPECT_EQ(1, sublist1->car()->as_number_int().as_int64());
    EXPECT_EQ(2, sublist1->cdr()->as_pair()->car()->as_number_int().as_int64());
}

TEST_F(ParserTest, ErrorHandling)
{
    EXPECT_THROW(parse("("), std::runtime_error);
    EXPECT_THROW(parse(")"), std::runtime_error);
    EXPECT_THROW(parse("()"), std::runtime_error);
}
