//
// Created by glom on 9/25/25.
//
#include <gtest/gtest.h>
#include "tokenizer.h"
#include "expr.h"

class TokenizerTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Before test
    }

    void TearDown() override
    {
        // After test
    }
};

TEST_F(TokenizerTest, BasicNumberParsing)
{
    Tokenizer tokenizer("123");
    const Token token = tokenizer.next();
    EXPECT_EQ(token.get_type(), TOKEN_NUMBER);
    EXPECT_DOUBLE_EQ(token.as_number(), 123.0);
}

TEST_F(TokenizerTest, DecimalNumberParsing)
{
    Tokenizer tokenizer("45.67");
    const Token token = tokenizer.next();
    EXPECT_EQ(token.get_type(), TOKEN_NUMBER);
    EXPECT_DOUBLE_EQ(token.as_number(), 45.67);
}

TEST_F(TokenizerTest, NegativeNumberParsing)
{
    Tokenizer tokenizer("-89");
    const Token token = tokenizer.next();
    EXPECT_EQ(token.get_type(), TOKEN_NUMBER);
    EXPECT_DOUBLE_EQ(token.as_number(), -89.0);
}

TEST_F(TokenizerTest, StringParsing)
{
    Tokenizer tokenizer("\"hello world\"");
    Token token = tokenizer.next();
    EXPECT_EQ(token.get_type(), TOKEN_STRING);
    EXPECT_EQ(token.as_string(), "hello world");
}

TEST_F(TokenizerTest, StringWithEscapeSequences)
{
    Tokenizer tokenizer(R"("hello\nworld\t!")");
    Token token = tokenizer.next();
    EXPECT_EQ(token.get_type(), TOKEN_STRING);
    EXPECT_EQ(token.as_string(), "hello\nworld\t!");
}

TEST_F(TokenizerTest, BooleanParsing)
{
    Tokenizer tokenizer("true false #t #f");

    Token token1 = tokenizer.next();
    EXPECT_EQ(token1.get_type(), TOKEN_BOOLEAN);
    EXPECT_TRUE(token1.as_boolean());

    Token token2 = tokenizer.next();
    EXPECT_EQ(token2.get_type(), TOKEN_BOOLEAN);
    EXPECT_FALSE(token2.as_boolean());

    Token token3 = tokenizer.next();
    EXPECT_EQ(token3.get_type(), TOKEN_BOOLEAN);
    EXPECT_TRUE(token3.as_boolean());

    Token token4 = tokenizer.next();
    EXPECT_EQ(token4.get_type(), TOKEN_BOOLEAN);
    EXPECT_FALSE(token4.as_boolean());
}

TEST_F(TokenizerTest, IdentifierParsing)
{
    Tokenizer tokenizer("abc define123 lambda? + - */");

    Token token1 = tokenizer.next();
    EXPECT_EQ(token1.get_type(), TOKEN_SYMBOL);
    EXPECT_EQ(token1.as_string(), "abc");

    Token token2 = tokenizer.next();
    EXPECT_EQ(token2.get_type(), TOKEN_SYMBOL);
    EXPECT_EQ(token2.as_string(), "define123");

    Token token3 = tokenizer.next();
    EXPECT_EQ(token3.get_type(), TOKEN_SYMBOL);
    EXPECT_EQ(token3.as_string(), "lambda?");

    Token token4 = tokenizer.next();
    EXPECT_EQ(token4.get_type(), TOKEN_SYMBOL);
    EXPECT_EQ(token4.as_string(), "+");

    Token token5 = tokenizer.next();
    EXPECT_EQ(token5.get_type(), TOKEN_SYMBOL);
    EXPECT_EQ(token5.as_string(), "-");

    Token token6 = tokenizer.next();
    EXPECT_EQ(token6.get_type(), TOKEN_SYMBOL);
    EXPECT_EQ(token6.as_string(), "*/");
}

TEST_F(TokenizerTest, Parentheses)
{
    Tokenizer tokenizer("() ( () )");

    EXPECT_EQ(tokenizer.next().get_type(), TOKEN_LPAREN);
    EXPECT_EQ(tokenizer.next().get_type(), TOKEN_RPAREN);
    EXPECT_EQ(tokenizer.next().get_type(), TOKEN_LPAREN);
    EXPECT_EQ(tokenizer.next().get_type(), TOKEN_LPAREN);
    EXPECT_EQ(tokenizer.next().get_type(), TOKEN_RPAREN);
    EXPECT_EQ(tokenizer.next().get_type(), TOKEN_RPAREN);
}

TEST_F(TokenizerTest, SkipWhitespace)
{
    Tokenizer tokenizer("  123  \n  \"abc\"  \t  true  ");

    EXPECT_EQ(tokenizer.next().get_type(), TOKEN_NUMBER);

    EXPECT_EQ(tokenizer.next().get_type(), TOKEN_STRING);

    EXPECT_EQ(tokenizer.next().get_type(), TOKEN_BOOLEAN);
}

TEST_F(TokenizerTest, Quote)
{
    Tokenizer tokenizer("'123 'abc '(a b c) '()");

    EXPECT_EQ(tokenizer.next().get_type(), TOKEN_QUOTE);
    EXPECT_EQ(tokenizer.next().get_type(), TOKEN_NUMBER);

    EXPECT_EQ(tokenizer.next().get_type(), TOKEN_QUOTE);
    EXPECT_EQ(tokenizer.next().get_type(), TOKEN_SYMBOL);

    EXPECT_EQ(tokenizer.next().get_type(), TOKEN_QUOTE);
    EXPECT_EQ(tokenizer.next().get_type(), TOKEN_LPAREN);
    EXPECT_EQ(tokenizer.next().get_type(), TOKEN_SYMBOL);
    EXPECT_EQ(tokenizer.next().get_type(), TOKEN_SYMBOL);
    EXPECT_EQ(tokenizer.next().get_type(), TOKEN_SYMBOL);
    EXPECT_EQ(tokenizer.next().get_type(), TOKEN_RPAREN);

    EXPECT_EQ(tokenizer.next().get_type(), TOKEN_QUOTE);
    EXPECT_EQ(tokenizer.next().get_type(), TOKEN_LPAREN);
    EXPECT_EQ(tokenizer.next().get_type(), TOKEN_RPAREN);
}

TEST_F(TokenizerTest, ComplexExpression)
{
    Tokenizer tokenizer("(define (fact n) (if (= n 0) 1 (* n (fact (- n 1)))))");

    const std::vector expected_types = {
        TOKEN_LPAREN,
            TOKEN_SYMBOL,
                TOKEN_LPAREN, TOKEN_SYMBOL, TOKEN_SYMBOL, TOKEN_RPAREN,
                TOKEN_LPAREN, TOKEN_SYMBOL,
                    TOKEN_LPAREN, TOKEN_SYMBOL, TOKEN_SYMBOL, TOKEN_NUMBER, TOKEN_RPAREN,
                    TOKEN_NUMBER,
                        TOKEN_LPAREN, TOKEN_SYMBOL, TOKEN_SYMBOL,
                            TOKEN_LPAREN, TOKEN_SYMBOL,
                                TOKEN_LPAREN, TOKEN_SYMBOL, TOKEN_SYMBOL, TOKEN_NUMBER, TOKEN_RPAREN,
                    TOKEN_RPAREN,
                TOKEN_RPAREN,
            TOKEN_RPAREN,
        TOKEN_RPAREN, TOKEN_EOI
    };

    std::vector<TokenType> actual_types;
    Token current = tokenizer.next();
    while (current.get_type() != TOKEN_EOI)
    {
        actual_types.push_back(current.get_type());
        current = tokenizer.next();
    }
    actual_types.push_back(current.get_type());

    EXPECT_EQ(actual_types, expected_types);
}

TEST_F(TokenizerTest, EndOfInput)
{
    Tokenizer tokenizer("");
    const Token token = tokenizer.next();
    EXPECT_EQ(token.get_type(), TOKEN_EOI);

    // 连续调用应该继续返回 EOI
    const Token token2 = tokenizer.next();
    EXPECT_EQ(token2.get_type(), TOKEN_EOI);
}

TEST_F(TokenizerTest, InvalidStringEscape)
{
    Tokenizer tokenizer(R"("hello\xworld")");
    EXPECT_THROW(tokenizer.next(), std::runtime_error);
}

TEST_F(TokenizerTest, UnterminatedString)
{
    Tokenizer tokenizer("\"hello world");
    EXPECT_THROW(tokenizer.next(), std::runtime_error);
}
