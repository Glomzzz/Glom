#include <gtest/gtest.h>
#include <memory>
#include <string>

#include "expr.h"
#include "context.h"
#include "parser.h"
#include "eval.h"
#include "error.h"

class SchemeMutContextTest : public ::testing::Test
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

// ---------- set! ----------

TEST_F(SchemeMutContextTest, Set_ReassignsExistingBinding)
{
    perform("(define x 10)");
    const auto r = eval("(set! x 42)");
    // R5RS: unspecified => we use Expr::NOTHING as sentinel
    EXPECT_EQ(Expr::NOTHING, r);

    const auto x = eval("x");
    EXPECT_TRUE(x->is_number_int());
    EXPECT_EQ(integer(42), x->as_number_int());
}

TEST_F(SchemeMutContextTest, Set_UnboundRaises)
{
    EXPECT_THROW(perform("(set! y 3)"), GlomError);
}

TEST_F(SchemeMutContextTest, Set_RespectsLexicalScope)
{
    // Inner let shadows outer x; set! in inner scope updates inner binding only.
    const auto res = eval("(begin (define x 1) (let ((x 2)) (set! x 3) x) x)");
    EXPECT_TRUE(res->is_number_int());
    // outer x remains 1
    EXPECT_EQ(integer(1), res->as_number_int());
}

TEST_F(SchemeMutContextTest, Set_UpdatesOuterWhenNotShadowed)
{
    const auto res = eval("(begin (define x 1) (let () (set! x 9)) x)");
    EXPECT_TRUE(res->is_number_int());
    EXPECT_EQ(integer(9), res->as_number_int());
}

// ---------- set-car! ----------

TEST_F(SchemeMutContextTest, SetCar_BasicMutation)
{
    perform("(define p (cons 1 2))");
    const auto r = eval("(set-car! p 99)");
    EXPECT_EQ(Expr::NOTHING, r);

    const auto p = eval("p");
    ASSERT_TRUE(p->is_pair());
    const auto pair = p->as_pair();
    ASSERT_NE(nullptr, pair);
    EXPECT_TRUE(pair->car()->is_number_int());
    EXPECT_EQ(integer(99), pair->car()->as_number_int());
    EXPECT_TRUE(pair->cdr()->is_number_int());
    EXPECT_EQ(integer(2), pair->cdr()->as_number_int());
}

TEST_F(SchemeMutContextTest, SetCar_PropagatesThroughAliases)
{
    perform("(define p (cons 7 8))");
    perform("(define q p)");
    perform("(set-car! q 11)");
    const auto p = eval("p");
    const auto q = eval("q");
    ASSERT_TRUE(p->is_pair() && q->is_pair());
    EXPECT_EQ(p->as_pair()->car()->as_number_int(), q->as_pair()->car()->as_number_int());
    EXPECT_EQ(integer(11), p->as_pair()->car()->as_number_int());
}

TEST_F(SchemeMutContextTest, SetCar_EmptyListError)
{
    EXPECT_THROW(perform("(set-car! '() 1)"), GlomError);
}

TEST_F(SchemeMutContextTest, SetCar_NonPairError)
{
    EXPECT_THROW(perform("(set-car! 123 0)"), GlomError);
}

// ---------- set-cdr! ----------

TEST_F(SchemeMutContextTest, SetCdr_BasicMutationToProperList)
{
    perform("(define p (cons 1 2))");
    const auto r = eval("(set-cdr! p '(3 4))");
    EXPECT_EQ(Expr::NOTHING, r);

    const auto p = eval("p");
    ASSERT_TRUE(p->is_pair());
    const auto s = p->to_string();
    // Should be (1 3 4)
    EXPECT_EQ("(1 3 4)", s);
}

TEST_F(SchemeMutContextTest, SetCdr_ToImproperList)
{
    perform("(define p (cons 1 2))");
    perform("(set-cdr! p 9)");
    const auto s = eval("p")->to_string();
    // Improper list: (1 . 9)
    EXPECT_EQ("(1 . 9)", s);
}

TEST_F(SchemeMutContextTest, SetCdr_EmptyListError)
{
    EXPECT_THROW(perform("(set-cdr! '() '(a b))"), GlomError);
}

TEST_F(SchemeMutContextTest, SetCdr_NonPairError)
{
    EXPECT_THROW(perform("(set-cdr! 123 '(x))"), GlomError);
}

// ---------- Integration: list mutations via variables ----------

TEST_F(SchemeMutContextTest, MutateListNodeInsideAList)
{
    perform("(define p (cons 1 (cons 2 (cons 3 '()))))"); // (1 2 3)
    perform("(define tail (cdr p))");                     // points to (2 3)
    perform("(set-car! tail 99)");                        // (1 99 3)
    perform("(set-cdr! tail (cons 100 '()))");            // tail becomes (99 100), full list (1 99 100)
    const auto s = eval("p")->to_string();
    EXPECT_EQ("(1 99 100)", s);
}

// ---------- Return value semantics ----------

TEST_F(SchemeMutContextTest, MutationProceduresReturnUnspecified)
{
    perform("(define x 0)");
    EXPECT_EQ(Expr::NOTHING, eval("(set! x 1)"));
    perform("(define p (cons 1 2))");
    EXPECT_EQ(Expr::NOTHING, eval("(set-car! p 7)"));
    EXPECT_EQ(Expr::NOTHING, eval("(set-cdr! p 8)"));
}
