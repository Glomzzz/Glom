#include <gtest/gtest.h>
#include "tokenizer.h"
#include "parser.h"

class ParserTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(ParserTest, AtomParsing) {

    vector<unique_ptr<Expr>> exprs = parse("123 \"hello\" true abc");

    EXPECT_EQ(4, exprs.size());

    EXPECT_EQ(ExprType::NUMBER, exprs[0]->type);
    EXPECT_EQ(123, std::get<double>(exprs[0]->value));

    EXPECT_EQ(ExprType::STRING, exprs[1]->type);
    EXPECT_EQ("hello", std::get<string>(exprs[1]->value));

    EXPECT_EQ(ExprType::BOOLEAN, exprs[2]->type);
    EXPECT_EQ(true, std::get<bool>(exprs[2]->value));

    EXPECT_EQ(ExprType::SYMBOL, exprs[3]->type);
    EXPECT_EQ("abc", std::get<string>(exprs[3]->value));
}

TEST_F(ParserTest, ListParsing) {
    vector<unique_ptr<Expr>> exprs = parse("(1 2 3)");
    const unique_ptr<Expr> list = std::move(exprs[0]);
    EXPECT_EQ(ExprType::LIST, list->type);
    const auto& elements = std::get<vector<unique_ptr<Expr>>>(list->value);
    EXPECT_EQ(3, elements.size());
    EXPECT_EQ(1, std::get<double>(elements[0]->value));
    EXPECT_EQ(2, std::get<double>(elements[1]->value));
    EXPECT_EQ(3, std::get<double>(elements[2]->value));
}

TEST_F(ParserTest, QuoteParsing) {
    vector<unique_ptr<Expr>> exprs = parse("'(1 2 3)");
    const unique_ptr<Expr> list = std::move(exprs[0]);
    EXPECT_EQ(ExprType::LIST, list->type);
    const auto& quote = std::get<vector<unique_ptr<Expr>>>(list->value);
    EXPECT_EQ(2, quote.size());
    EXPECT_EQ("quote", std::get<string>(quote[0]->value));
    const auto& elements = std::get<vector<unique_ptr<Expr>>>(quote[1]->value);
    EXPECT_EQ(3, elements.size());
    EXPECT_EQ(1, std::get<double>(elements[0]->value));
    EXPECT_EQ(2, std::get<double>(elements[1]->value));
    EXPECT_EQ(3, std::get<double>(elements[2]->value));
}

TEST_F(ParserTest, NestedList) {
    vector<unique_ptr<Expr>> exprs = parse("((1 2) (3 4))");
    const unique_ptr<Expr> list = std::move(exprs[0]);
    EXPECT_EQ(ExprType::LIST, list->type);
    const auto& elements = std::get<vector<unique_ptr<Expr>>>(list->value);
    EXPECT_EQ(2, elements.size());
    EXPECT_EQ(ExprType::LIST, elements[0]->type);
    const auto& sublist1 = std::get<vector<unique_ptr<Expr>>>(elements[0]->value);
    EXPECT_EQ(2, sublist1.size());
    EXPECT_EQ(1, std::get<double>(sublist1[0]->value));
    EXPECT_EQ(2, std::get<double>(sublist1[1]->value));
}

TEST_F(ParserTest, ErrorHandling) {
    EXPECT_THROW(parse("("), std::runtime_error);
    EXPECT_THROW(parse(")"), std::runtime_error);
}