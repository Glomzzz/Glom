#include <gtest/gtest.h>
#include "tokenizer.h"
#include "parser.h"
#include "expr.h"

class ParserTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(ParserTest, AtomParsing) {

    vector<shared_ptr<Expr>> exprs = parse("123 \"hello\" true abc");

    EXPECT_EQ(4, exprs.size());

    EXPECT_EQ(NUMBER, exprs[0]->get_type());
    EXPECT_EQ(123, exprs[0]->as_number());

    EXPECT_EQ(STRING, exprs[1]->get_type());
    EXPECT_EQ("hello", exprs[1]->as_string());

    EXPECT_EQ(BOOLEAN, exprs[2]->get_type());
    EXPECT_EQ(true, exprs[2]->as_boolean());

    EXPECT_EQ(SYMBOL, exprs[3]->get_type());
    EXPECT_EQ("abc", exprs[3]->as_symbol());
}

TEST_F(ParserTest, ListParsing) {
    vector<shared_ptr<Expr>> exprs = parse("(1 2 3)");
    const shared_ptr<Expr> list = std::move(exprs[0]);
    EXPECT_EQ(PAIR, list->get_type());
    int index = 0;
    for (const auto elem : *list->as_pair())
    {
        if (!elem) break;
        EXPECT_EQ(++index, elem->as_number());
    }
}

TEST_F(ParserTest, QuoteParsing) {
    vector<shared_ptr<Expr>> exprs = parse("'(1 2 3)");
    const shared_ptr<Expr> list = std::move(exprs[0]);
    EXPECT_EQ(PAIR, list->get_type());
    const auto& quote = list->as_pair();
    EXPECT_EQ("quote", quote->car()->as_symbol());
    const auto& elements = quote->cdr()->as_pair()->car()->as_pair();
    int index = 0;
    for (const auto elem : *elements)
    {
        if (!elem) break;
        EXPECT_EQ(++index, elem->as_number());
    }
}

TEST_F(ParserTest, NilParsing) {
    vector<shared_ptr<Expr>> exprs = parse("'()");
    const shared_ptr<Expr> quoted_nil = std::move(exprs[0]);
    EXPECT_EQ(PAIR, quoted_nil->get_type());
    const auto& quote = quoted_nil->as_pair();
    EXPECT_EQ("quote", quote->car()->as_symbol());
    const auto& nil_nil = quote->cdr()->as_pair();
    EXPECT_EQ(Expr::NIL, nil_nil->car());
    EXPECT_EQ(Expr::NIL, nil_nil->cdr());

}


TEST_F(ParserTest, NestedList) {
    vector<shared_ptr<Expr>> exprs = parse("((1 2) (3 4))");
    const shared_ptr<Expr> list = std::move(exprs[0]);
    EXPECT_EQ(PAIR, list->get_type());
    const auto& elements = list->as_pair();
    EXPECT_EQ(PAIR, elements->car()->get_type());
    const auto& sublist1 = elements->car()->as_pair();
    EXPECT_EQ(1, sublist1->car()->as_number());
    EXPECT_EQ(2, sublist1->cdr()->as_pair()->car()->as_number());
}

TEST_F(ParserTest, ErrorHandling) {
    EXPECT_THROW(parse("("), std::runtime_error);
    EXPECT_THROW(parse(")"), std::runtime_error);
    EXPECT_THROW(parse("()"), std::runtime_error);
}