#include <gtest/gtest.h>
#include <vector>
#include <memory>

#include "expr.h"
#include "context.h"
#include "parser.h"
#include "eval.h"

class SchemePrimitivesTest : public ::testing::Test
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

// Quote operation tests
TEST_F(SchemePrimitivesTest, Quote)
{
    // Quote should return the quoted expression without evaluation
    const auto result1 = eval("(quote a)");
    EXPECT_EQ(SYMBOL, result1->get_type());
    EXPECT_EQ("a", view_to_string(result1->as_symbol()));

    const auto result2 = eval("(quote (1 2 3))");
    EXPECT_EQ(PAIR, result2->get_type());

    const auto result3 = eval("'a"); // shorthand for quote
    EXPECT_EQ(SYMBOL, result3->get_type());
    EXPECT_EQ("a", view_to_string(result3->as_symbol()));

    const auto result4 = eval("'()");
    EXPECT_TRUE(result4->is_nil());
}

// Lambda tests
TEST_F(SchemePrimitivesTest, LambdaCreation)
{
    // Lambda should create a lambda expression
    const auto result = eval("(lambda (x) (+ x 1))");
    EXPECT_EQ(LAMBDA, result->get_type());
}

TEST_F(SchemePrimitivesTest, LambdaApplication)
{
    // Lambda application
    perform("(define add1 (lambda (x) (+ x 1)))");
    EXPECT_EQ(integer(6),eval("(add1 5)")->as_number_int());

    perform("(define add (lambda (x y) (+ x y)))");
    EXPECT_EQ(integer(7),eval("(add 3 4)")->as_number_int());
}

// Binding tests
TEST_F(SchemePrimitivesTest, Define)
{
    // Define should bind values
    perform("(define x 5)");
    const auto x = eval("x");
    EXPECT_EQ(NUMBER_INT, x->get_type());
    EXPECT_EQ(integer(5),x->as_number_int());

    perform("(define y (+ 2 3))");
    const auto y = eval("y");
    EXPECT_EQ(NUMBER_INT, y->get_type());
    EXPECT_EQ(integer(5),y->as_number_int());

    perform("(define (double x) (* 2 x))");
    const auto double_5 = eval("(double 5)");
    EXPECT_EQ(NUMBER_INT, double_5->get_type());
    EXPECT_EQ(integer(10),double_5->as_number_int());
}

TEST_F(SchemePrimitivesTest, Let)
{
    // Let should create local bindings
    EXPECT_EQ(integer(7),eval("(let ((x 2) (y 5)) (+ x y))")->as_number_int());
    EXPECT_EQ(integer(50),eval("(let ((x 5)) (* x 10))")->as_number_int());

    std::cout << eval("(let f ((a 10) (b 15)) (if (= a 0) 0 (+ a b (f (- a 1) b))))")->to_string() << std::endl;
    // EXPECT_EQ(integer(15),eval("(let f ((a 10) (b 15)) (if (= a 0) 0 (+ a b (f (- a 1) b))))")->as_number_int());

    // Let should not leak bindings
    perform("(let ((z 100)) z)");
    EXPECT_ANY_THROW(perform("z"));
}


// Logic operations tests
TEST_F(SchemePrimitivesTest, LogicalAnd)
{
    EXPECT_EQ(Expr::TRUE, eval("(and #t #t)"));
    EXPECT_EQ(Expr::FALSE, eval("(and #t #f)"));
    EXPECT_EQ(Expr::FALSE, eval("(and #f #t)"));
    EXPECT_EQ(Expr::FALSE, eval("(and #f #f)"));
    EXPECT_EQ(Expr::TRUE, eval("(and 1 2 3)"));
    EXPECT_EQ(Expr::FALSE, eval("(and 1 #f 3)"));
    EXPECT_EQ(Expr::TRUE, eval("(and)"));
}

TEST_F(SchemePrimitivesTest, LogicalOr)
{
    EXPECT_EQ(Expr::TRUE, eval("(or #t #t)"));
    EXPECT_EQ(Expr::TRUE, eval("(or #t #f)"));
    EXPECT_EQ(Expr::TRUE, eval("(or #f #t)"));
    EXPECT_EQ(Expr::FALSE, eval("(or #f #f)"));
    EXPECT_EQ(Expr::TRUE, eval("(or 1 2 3)"));
    EXPECT_EQ(Expr::TRUE, eval("(or #f 2 #f)"));
    EXPECT_EQ(Expr::FALSE, eval("(or)"));
}

TEST_F(SchemePrimitivesTest, LogicalNot)
{
    EXPECT_EQ(Expr::TRUE, eval("(not #f)"));
    EXPECT_EQ(Expr::FALSE, eval("(not #t)"));
    EXPECT_EQ(Expr::FALSE, eval("(not 1)"));
    EXPECT_EQ(Expr::FALSE, eval("(not '())"));
}

// Pointer comparison tests
TEST_F(SchemePrimitivesTest, EqPointer)
{
    // eq? tests object identity
    EXPECT_EQ(Expr::TRUE, eval("(eq? 'a 'a)"));
    EXPECT_EQ(Expr::TRUE, eval("(eq? #t #t)"));
    EXPECT_EQ(Expr::TRUE, eval("(eq? #f #f)"));

    perform("(define x '(1 2 3))");
    perform("(define y x)");
    EXPECT_EQ(Expr::TRUE, eval("(eq? x y)"));

    perform("(define z '(1 2 3))");
    // x and z are different objects, so eq? should be false
    EXPECT_EQ(Expr::FALSE, eval("(eq? x z)"));
}

TEST_F(SchemePrimitivesTest, EqvValue)
{
    // eqv? tests for equivalent values
    EXPECT_EQ(Expr::TRUE, eval("(eqv? 1 1)"));
    EXPECT_EQ(Expr::TRUE, eval("(eqv? 1.0 1.0)"));
    EXPECT_EQ(Expr::TRUE, eval("(eqv? #t #t)"));
    EXPECT_EQ(Expr::TRUE, eval("(eqv? #f #f)"));
    EXPECT_EQ(Expr::TRUE, eval("(eqv? 1 1.0)"));
    EXPECT_EQ(Expr::FALSE, eval("(eqv? 1 2)"));
    EXPECT_EQ(Expr::FALSE, eval("(eqv? 'a 'b)"));
}

TEST_F(SchemePrimitivesTest, EqualStructure)
{
    // equal? tests for structural equality
    EXPECT_EQ(Expr::TRUE, eval("(equal? 1 1)"));
    EXPECT_EQ(Expr::TRUE, eval("(equal? '(1 2 3) '(1 2 3))"));
    EXPECT_EQ(Expr::TRUE, eval("(equal? \"hello\" \"hello\")"));
    EXPECT_EQ(Expr::FALSE, eval("(equal? '(1 2 3) '(1 2))"));
}

// Condition tests
TEST_F(SchemePrimitivesTest, IfCondition)
{
    EXPECT_EQ(integer(1),eval("(if #t 1 2)")->as_number_int());
    EXPECT_EQ(integer(2),eval("(if #f 1 2)")->as_number_int());
    EXPECT_EQ(integer(1),eval("(if (< 1 2) 1 2)")->as_number_int());
    EXPECT_EQ(integer(2),eval("(if (> 1 2) 1 2)")->as_number_int());

    // Test without alternative
    EXPECT_EQ(Expr::NOTHING, eval("(if #f 1)"));
}

TEST_F(SchemePrimitivesTest, CondCondition)
{
    const auto result_1 = eval("(cond (#t 1))");
    EXPECT_EQ(NUMBER_INT, result_1->get_type());
    EXPECT_EQ(integer(1), result_1->as_number_int());
    const auto result_2 = eval("(cond (#f 1) (#t 2))");
    EXPECT_EQ(NUMBER_INT, result_2->get_type());
    EXPECT_EQ(integer(2), result_2->as_number_int());
    const auto result_3 = eval("(cond (#f 1) (#f 2) (#t 3))");
    EXPECT_EQ(NUMBER_INT, result_3->get_type());
    EXPECT_EQ(integer(3), result_3->as_number_int());
    // Test with no true conditions
    const auto result_nothing = eval("(cond (#f 1) (#f 2) (#f 3))");
    EXPECT_EQ(Expr::NOTHING, result_nothing);

    // Test with predicates
    const auto result_2_ = eval("(cond ((< 1 0) 1) ((> 1 0) 2))");
    EXPECT_EQ(integer(2),result_2_->as_number_int());
}

// Apply tests
TEST_F(SchemePrimitivesTest, Apply)
{
    const auto result = eval("(apply + '(1 2 3 4))");
    EXPECT_EQ(NUMBER_INT, result->get_type());
    EXPECT_EQ(integer(10), result->as_number_int());
}
// List operations tests
TEST_F(SchemePrimitivesTest, Cons)
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

TEST_F(SchemePrimitivesTest, Car)
{
    EXPECT_EQ(integer(1), eval("(car '(1 2 3))")->as_number_int());
    EXPECT_EQ(integer(1), eval("(car (cons 1 2))")->as_number_int());
}

TEST_F(SchemePrimitivesTest, Cdr)
{
    const auto result = eval("(cdr '(1 2 3))");
    EXPECT_EQ(PAIR, result->get_type());
    EXPECT_EQ(integer(2), result->as_pair()->car()->as_number_int());

    EXPECT_EQ(integer(2), eval("(cdr (cons 1 2))")->as_number_int());
}

TEST_F(SchemePrimitivesTest, List)
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

TEST_F(SchemePrimitivesTest, IsNull)
{
    EXPECT_EQ(Expr::TRUE, eval("(null? '())"));
    EXPECT_EQ(Expr::FALSE, eval("(null? '(1 2 3))"));
    EXPECT_EQ(Expr::FALSE, eval("(null? 1)"));
    EXPECT_EQ(Expr::FALSE, eval("(null? #t)"));
}

// Complex integration tests
TEST_F(SchemePrimitivesTest, Factorial)
{
    perform(R"(
        (define (factorial n)
            (if (= n 0)
                1
                (* n (factorial (- n 1)))))
    )");

    EXPECT_EQ(integer(1),eval("(factorial 0)")->as_number_int());
    EXPECT_EQ(integer(1),eval("(factorial 1)")->as_number_int());
    EXPECT_EQ(integer(2),eval("(factorial 2)")->as_number_int());
    EXPECT_EQ(integer(6),eval("(factorial 3)")->as_number_int());
    EXPECT_EQ(integer(24),eval("(factorial 4)")->as_number_int());
}

TEST_F(SchemePrimitivesTest, Fibonacci)
{
    perform(R"(
        (define (fib n)
            (cond ((= n 0) 0)
                  ((= n 1) 1)
                  (else (+ (fib (- n 1)) (fib (- n 2))))))
    )");

    EXPECT_EQ(integer(0),eval("(fib 0)")->as_number_int());
    EXPECT_EQ(integer(1),eval("(fib 1)")->as_number_int());
    EXPECT_EQ(integer(1),eval("(fib 2)")->as_number_int());
    EXPECT_EQ(integer(2),eval("(fib 3)")->as_number_int());
    EXPECT_EQ(integer(3),eval("(fib 4)")->as_number_int());
}

TEST_F(SchemePrimitivesTest, DefineMapFunction)
{
    perform("(define (double x) (* 2 x))");
    perform("(define (map proc list) (if (null? list) '() (cons (proc (car list)) (map proc (cdr list)))))");

    // This tests complex list manipulation
    const auto result = eval("(map double '(1 2 3 4))");
    EXPECT_EQ(PAIR, result->get_type());

    auto pair = result->as_pair();
    EXPECT_EQ(integer(2),pair->car()->as_number_int());

    pair = pair->cdr()->as_pair();
    EXPECT_EQ(integer(4),pair->car()->as_number_int());

    pair = pair->cdr()->as_pair();
    EXPECT_EQ(integer(6),pair->car()->as_number_int());

    pair = pair->cdr()->as_pair();
    EXPECT_EQ(integer(8),pair->car()->as_number_int());
    EXPECT_TRUE(pair->cdr()->is_nil());
}


int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
