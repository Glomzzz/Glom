#include <gtest/gtest.h>
#include <memory>
#include <string>

#include "expr.h"
#include "context.h"
#include "parser.h"
#include "eval.h"
#include "error.h"

using std::shared_ptr;
using std::string;

class DelayForceTest : public ::testing::Test {
protected:
    void SetUp() override { ctx = make_root_context(); }
    void TearDown() override { ctx.reset(); }

    [[nodiscard]] shared_ptr<Expr> eval(const string& src) const {
        const auto exprs = parse(src);
        return ::eval(ctx, exprs);
    }

    void perform(const string& src) const {
        const auto exprs = parse(src);
        ::eval(ctx, exprs);
    }

    shared_ptr<Context> ctx;
};

// ---- Basics ----

TEST_F(DelayForceTest, DelayReturnsPromiseObject)
{
    const auto p = eval("(delay 123)");
    // Representation detail: a promise is a tagged pair.
    ASSERT_TRUE(p->is_pair());
    // Don't assert the exact printed form; just that forcing yields value:
    const auto v = eval("(force (delay 123))");
    ASSERT_TRUE(v->is_number_int());
    EXPECT_EQ(integer(123), v->as_number_int());
}

TEST_F(DelayForceTest, ForceEvaluatesOnceAndMemoizes)
{
    perform("(define c 0)");
    perform("(define p (delay (begin (set! c (+ c 1)) 42)))");
    // Not evaluated yet:
    EXPECT_TRUE(eval("c")->is_number_int());
    EXPECT_EQ(integer(0), eval("c")->as_number_int());

    // First force: computes value and increments c
    const auto v1 = eval("(force p)");
    ASSERT_TRUE(v1->is_number_int());
    EXPECT_EQ(integer(42), v1->as_number_int());
    EXPECT_EQ(integer(1), eval("c")->as_number_int());

    // Second force: returns memoized value; no more increments
    const auto v2 = eval("(force p)");
    ASSERT_TRUE(v2->is_number_int());
    EXPECT_EQ(integer(42), v2->as_number_int());
    EXPECT_EQ(integer(1), eval("c")->as_number_int());
}

TEST_F(DelayForceTest, ForceArgumentIsEvaluated)
{
    // (force ((lambda () (delay 7)))) -> 7
    const auto v = eval("(force ((lambda () (delay 7))))");
    ASSERT_TRUE(v->is_number_int());
    EXPECT_EQ(integer(7), v->as_number_int());
}

// ---- Errors ----

TEST_F(DelayForceTest, DelayRequiresExactlyOneExpression)
{
    EXPECT_THROW(perform("(delay)"), GlomError);
    EXPECT_THROW(perform("(delay 1 2)"), GlomError);
}

TEST_F(DelayForceTest, ForceRequiresPromise)
{
    EXPECT_THROW(perform("(force 5)"), GlomError);
    EXPECT_THROW(perform("(force '(1 2 3))"), GlomError);
}

// ---- Sharing / Aliasing ----

TEST_F(DelayForceTest, SharedPromiseOnlyComputesOnce)
{
    perform("(define c 0)");
    perform("(define p (delay (begin (set! c (+ c 1)) 99)))");
    perform("(define q p)");
    EXPECT_EQ(integer(99), eval("(force p)")->as_number_int());
    EXPECT_EQ(integer(1),  eval("c")->as_number_int());
    EXPECT_EQ(integer(99), eval("(force q)")->as_number_int());
    EXPECT_EQ(integer(1),  eval("c")->as_number_int());
}

// ---- Promise returning another promise (no auto-chaining) ----

TEST_F(DelayForceTest, ForceDoesNotAutoForceNestedPromise)
{
    perform("(define p (delay (delay 5)))");
    // Forcing p yields a *promise* result, not 5 (no auto-chaining)
    const auto r = eval("(force p)");
    ASSERT_TRUE(r->is_pair()); // our promise representation
    // If you want transitive forcing, users can do (force (force p))
    const auto v = eval("(force (force p))");
    ASSERT_TRUE(v->is_number_int());
    EXPECT_EQ(integer(5), v->as_number_int());
}
