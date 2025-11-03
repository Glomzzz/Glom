#include <gtest/gtest.h>
#include <memory>
#include <string>

#include "expr.h"
#include "context.h"
#include "parser.h"
#include "eval.h"
#include "error.h"

class SchemeEvalControlTest : public ::testing::Test
{
protected:
    void SetUp() override { context = make_root_context(); }
    void TearDown() override { context.reset(); }

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

    std::shared_ptr<Context> context;
};

// ---------------- Presence / binding sanity ----------------

TEST_F(SchemeEvalControlTest, PrimitivesAreBoundInRoot)
{
    // They should be present as procedure-like values.
    EXPECT_TRUE(eval("begin")->is_primitive());
    EXPECT_TRUE(eval("apply")->is_primitive());
    EXPECT_TRUE(eval("call/cc")->is_primitive());
    EXPECT_TRUE(eval("error")->is_primitive());
}

// ---------------- begin ----------------

TEST_F(SchemeEvalControlTest, Begin_EmptyReturnsUnspecified)
{
    EXPECT_EQ(Expr::NOTHING, eval("(begin)"));
}

TEST_F(SchemeEvalControlTest, Begin_SequencesAndReturnsLast)
{
    perform("(define x 0)");
    const auto r = eval("(begin (set! x 5) (+ x 7))");
    ASSERT_TRUE(r->is_number_int());
    EXPECT_EQ(integer(12), r->as_number_int());
    EXPECT_EQ(integer(5), eval("x")->as_number_int());
}

// ---------------- apply ----------------

TEST_F(SchemeEvalControlTest, Apply_OnPrimitiveProc)
{
    const auto r = eval("(apply + '(1 2 3 4))");
    ASSERT_TRUE(r->is_number_int());
    EXPECT_EQ(integer(10), r->as_number_int());
}

TEST_F(SchemeEvalControlTest, Apply_OnLambda)
{
    const auto r = eval("(apply (lambda (a b) (+ a b)) '(4 5))");
    ASSERT_TRUE(r->is_number_int());
    EXPECT_EQ(integer(9), r->as_number_int());
}

TEST_F(SchemeEvalControlTest, Apply_TypeErrorWhenFirstNotProc)
{
    EXPECT_THROW(perform("(apply 123 '(1 2))"), GlomError);
}

// ---------------- call/cc ----------------

TEST_F(SchemeEvalControlTest, CallCC_RequiresProcOfArityOne)
{
    // Not a procedure
    EXPECT_THROW(perform("(call/cc 1)"), GlomError);
    // Wrong arity
    EXPECT_THROW(perform("(call/cc (lambda (a b) 0))"), GlomError);
}

TEST_F(SchemeEvalControlTest, CallCC_SimpleReturnWhenIgnoringK)
{
    // If the lambda ignores k and returns 5, call/cc should yield 5.
    const auto r = eval("(call/cc (lambda (k) 5))");
    ASSERT_TRUE(r->is_number_int());
    EXPECT_EQ(integer(5), r->as_number_int());
}

TEST_F(SchemeEvalControlTest, CallCC_EscapeContinuation)
{
    // Classic escape: immediately invoke k to return 99; the rest is skipped.
    const auto r = eval("(call/cc (lambda (k) (k 99) 0))");
    ASSERT_TRUE(r->is_number_int());
    EXPECT_EQ(integer(99), r->as_number_int());
}

// ---------------- error ----------------

TEST_F(SchemeEvalControlTest, Error_RaisesGlomErrorWithMessage)
{
    try {
        (void)eval(R"((error "oops"))");
        FAIL() << "Expected GlomError";
    } catch (const GlomError& e) {
        // Your implementation prefixes "Error: "
        const std::string msg = e.what();
        EXPECT_NE(msg.find("Error:"), std::string::npos);
        EXPECT_NE(msg.find("oops"), std::string::npos);
    }
}

TEST_F(SchemeEvalControlTest, Error_ConcatenatesArgsToMessage)
{
    try {
        (void)eval("(error 1 \"two\" 'three)");
        FAIL() << "Expected GlomError";
    } catch (const GlomError& e) {
        const std::string msg = e.what();
        // Should contain printed forms of all args
        EXPECT_NE(msg.find("1"), std::string::npos);
        EXPECT_NE(msg.find("two"), std::string::npos);
        EXPECT_NE(msg.find("three"), std::string::npos);
    }
}