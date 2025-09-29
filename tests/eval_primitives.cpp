#include <gtest/gtest.h>
#include <vector>
#include <memory>

#include "expr.h"
#include "context.h"
#include "parser.h"

class SchemePrimitivesTest : public ::testing::Test {
protected:
    void SetUp() override {
        context = make_root_context();
    }

    void TearDown() override {
        context.reset();
    }

    shared_ptr<Expr> eval(const std::string& input) const
    {
        const auto exprs = parse(input);
        return context->eval(exprs);
    }

    void perform(const std::string& input) const
    {
        const auto exprs = parse(input);
        context->eval(exprs);
    }

    static shared_ptr<Expr> parse_and_get_first(const std::string& input) {
        auto exprs = parse(input);
        if (exprs.empty()) return Expr::NOTHING;
        return exprs[0];
    }

    std::shared_ptr<Context> context;
};

// Number operations tests
TEST_F(SchemePrimitivesTest, Addition) {
    // Basic addition
    EXPECT_DOUBLE_EQ(3.0, eval("(+ 1 2)")->as_number());
    EXPECT_DOUBLE_EQ(10.0, eval("(+ 1 2 3 4)")->as_number());
    EXPECT_DOUBLE_EQ(0.0, eval("(+)")->as_number());
    EXPECT_DOUBLE_EQ(5.0, eval("(+ 5)")->as_number());
}

TEST_F(SchemePrimitivesTest, Multiplication) {
    // Basic multiplication
    EXPECT_DOUBLE_EQ(6.0, eval("(* 2 3)")->as_number());
    EXPECT_DOUBLE_EQ(24.0, eval("(* 1 2 3 4)")->as_number());
    EXPECT_DOUBLE_EQ(1.0, eval("(*)")->as_number());
    EXPECT_DOUBLE_EQ(5.0, eval("(* 5)")->as_number());
}

TEST_F(SchemePrimitivesTest, Subtraction) {
    // Basic subtraction
    EXPECT_DOUBLE_EQ(1.0, eval("(- 3 2)")->as_number());
    EXPECT_DOUBLE_EQ(-8.0, eval("(- 1 2 3 4)")->as_number());
    EXPECT_DOUBLE_EQ(-5.0, eval("(- 5)")->as_number());
}

TEST_F(SchemePrimitivesTest, Division) {
    // Basic division
    EXPECT_DOUBLE_EQ(2.0, eval("(/ 6 3)")->as_number());
    EXPECT_DOUBLE_EQ(2.0, eval("(/ 24 3 4)")->as_number());
    EXPECT_DOUBLE_EQ(0.2, eval("(/ 5)")->as_number());

    // Division by zero should be handled
    // This depends on your implementation - might throw or return inf
}

TEST_F(SchemePrimitivesTest, Remainder) {
    // Basic remainder
    EXPECT_DOUBLE_EQ(1.0, eval("(remainder 5 2)")->as_number());
    EXPECT_DOUBLE_EQ(0.0, eval("(remainder 4 2)")->as_number());
    EXPECT_DOUBLE_EQ(2.0, eval("(remainder 2 3)")->as_number());
}

// Quote operation tests
TEST_F(SchemePrimitivesTest, Quote) {
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
TEST_F(SchemePrimitivesTest, LambdaCreation) {
    // Lambda should create a lambda expression
    const auto result = eval("(lambda (x) (+ x 1))");
    EXPECT_EQ(LAMBDA, result->get_type());
}

TEST_F(SchemePrimitivesTest, LambdaApplication) {
    // Lambda application
    perform("(define add1 (lambda (x) (+ x 1)))");
    EXPECT_DOUBLE_EQ(6.0, eval("(add1 5)")->as_number());

    perform("(define add (lambda (x y) (+ x y)))");
    EXPECT_DOUBLE_EQ(7.0, eval("(add 3 4)")->as_number());
}

// Binding tests
TEST_F(SchemePrimitivesTest, Define) {
    // Define should bind values
    perform("(define x 5)");
    const auto x = eval("x");
    EXPECT_EQ(NUMBER, x->get_type());
    EXPECT_DOUBLE_EQ(5.0, x->as_number());

    perform("(define y (+ 2 3))");
    const auto y = eval("y");
    EXPECT_EQ(NUMBER, y->get_type());
    EXPECT_DOUBLE_EQ(5.0, y->as_number());

    perform("(define (double x) (* 2 x))");
    const auto double_5 = eval("(double 5)");
    EXPECT_EQ(NUMBER, double_5->get_type());
    EXPECT_DOUBLE_EQ(10.0, double_5->as_number());
}

TEST_F(SchemePrimitivesTest, Let) {
    // Let should create local bindings
    EXPECT_DOUBLE_EQ(7.0, eval("(let ((x 2) (y 5)) (+ x y))")->as_number());
    EXPECT_DOUBLE_EQ(50.0, eval("(let ((x 5)) (* x 10))")->as_number());

    // Let should not leak bindings
    perform("(let ((z 100)) z)");
    EXPECT_ANY_THROW(eval("z"));
}

// Number comparison tests
TEST_F(SchemePrimitivesTest, NumericEquality) {
    EXPECT_EQ(Expr::TRUE, eval("(= 1 1)"));
    EXPECT_EQ(Expr::TRUE, eval("(= 1.0 1)"));
    EXPECT_EQ(Expr::FALSE, eval("(= 1 2)"));
    EXPECT_EQ(Expr::TRUE, eval("(= 1 1 1)"));
    EXPECT_EQ(Expr::FALSE, eval("(= 1 1 2)"));
}

TEST_F(SchemePrimitivesTest, LessThan) {
    EXPECT_EQ(Expr::TRUE, eval("(< 1 2)"));
    EXPECT_EQ(Expr::FALSE, eval("(< 2 1)"));
    EXPECT_EQ(Expr::FALSE, eval("(< 1 1)"));
    EXPECT_EQ(Expr::TRUE, eval("(< 1 2 3)"));
    EXPECT_EQ(Expr::FALSE, eval("(< 1 3 2)"));
}

TEST_F(SchemePrimitivesTest, GreaterThan) {
    EXPECT_EQ(Expr::TRUE, eval("(> 2 1)"));
    EXPECT_EQ(Expr::FALSE, eval("(> 1 2)"));
    EXPECT_EQ(Expr::FALSE, eval("(> 1 1)"));
    EXPECT_EQ(Expr::TRUE, eval("(> 3 2 1)"));
    EXPECT_EQ(Expr::FALSE, eval("(> 3 1 2)"));
}

TEST_F(SchemePrimitivesTest, LessThanOrEqual) {
    EXPECT_EQ(Expr::TRUE, eval("(<= 1 2)"));
    EXPECT_EQ(Expr::FALSE, eval("(<= 2 1)"));
    EXPECT_EQ(Expr::TRUE, eval("(<= 1 1)"));
    EXPECT_EQ(Expr::TRUE, eval("(<= 1 1 2)"));
    EXPECT_EQ(Expr::FALSE, eval("(<= 1 2 1)"));
}

TEST_F(SchemePrimitivesTest, GreaterThanOrEqual) {
    EXPECT_EQ(Expr::TRUE, eval("(>= 2 1)"));
    EXPECT_EQ(Expr::FALSE, eval("(>= 1 2)"));
    EXPECT_EQ(Expr::TRUE, eval("(>= 1 1)"));
    EXPECT_EQ(Expr::TRUE, eval("(>= 3 2 1)"));
    EXPECT_EQ(Expr::FALSE, eval("(>= 1 2 1)"));
}

// Logic operations tests
TEST_F(SchemePrimitivesTest, LogicalAnd) {
    EXPECT_EQ(Expr::TRUE, eval("(and #t #t)"));
    EXPECT_EQ(Expr::FALSE, eval("(and #t #f)"));
    EXPECT_EQ(Expr::FALSE, eval("(and #f #t)"));
    EXPECT_EQ(Expr::FALSE, eval("(and #f #f)"));
    EXPECT_EQ(Expr::TRUE, eval("(and 1 2 3)"));
    EXPECT_EQ(Expr::FALSE, eval("(and 1 #f 3)"));
    EXPECT_EQ(Expr::TRUE, eval("(and)"));
}

TEST_F(SchemePrimitivesTest, LogicalOr) {
    EXPECT_EQ(Expr::TRUE, eval("(or #t #t)"));
    EXPECT_EQ(Expr::TRUE, eval("(or #t #f)"));
    EXPECT_EQ(Expr::TRUE, eval("(or #f #t)"));
    EXPECT_EQ(Expr::FALSE, eval("(or #f #f)"));
    EXPECT_EQ(Expr::TRUE, eval("(or 1 2 3)"));
    EXPECT_EQ(Expr::TRUE, eval("(or #f 2 #f)"));
    EXPECT_EQ(Expr::FALSE, eval("(or)"));
}

TEST_F(SchemePrimitivesTest, LogicalNot) {
    EXPECT_EQ(Expr::TRUE, eval("(not #f)"));
    EXPECT_EQ(Expr::FALSE, eval("(not #t)"));
    EXPECT_EQ(Expr::FALSE, eval("(not 1)"));
    EXPECT_EQ(Expr::FALSE, eval("(not '())"));
}

// Pointer comparison tests
TEST_F(SchemePrimitivesTest, EqPointer) {
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

TEST_F(SchemePrimitivesTest, EqvValue) {
    // eqv? tests for equivalent values
    EXPECT_EQ(Expr::TRUE, eval("(eqv? 1 1)"));
    EXPECT_EQ(Expr::TRUE, eval("(eqv? 1.0 1.0)"));
    EXPECT_EQ(Expr::TRUE, eval("(eqv? #t #t)"));
    EXPECT_EQ(Expr::TRUE, eval("(eqv? #f #f)"));
    EXPECT_EQ(Expr::TRUE, eval("(eqv? 1 1.0)"));
    EXPECT_EQ(Expr::FALSE, eval("(eqv? 1 2)"));
    EXPECT_EQ(Expr::FALSE, eval("(eqv? 'a 'b)"));
}

TEST_F(SchemePrimitivesTest, EqualStructure) {
    // equal? tests for structural equality
    EXPECT_EQ(Expr::TRUE, eval("(equal? 1 1)"));
    EXPECT_EQ(Expr::TRUE, eval("(equal? '(1 2 3) '(1 2 3))"));
    EXPECT_EQ(Expr::TRUE, eval("(equal? \"hello\" \"hello\")"));
    EXPECT_EQ(Expr::FALSE, eval("(equal? '(1 2 3) '(1 2))"));
}

// Condition tests
TEST_F(SchemePrimitivesTest, IfCondition) {
    EXPECT_DOUBLE_EQ(1.0, eval("(if #t 1 2)")->as_number());
    EXPECT_DOUBLE_EQ(2.0, eval("(if #f 1 2)")->as_number());
    EXPECT_DOUBLE_EQ(1.0, eval("(if (< 1 2) 1 2)")->as_number());
    EXPECT_DOUBLE_EQ(2.0, eval("(if (> 1 2) 1 2)")->as_number());

    // Test without alternative
    EXPECT_EQ(Expr::NOTHING, eval("(if #f 1)"));
}

TEST_F(SchemePrimitivesTest, CondCondition) {
    const auto result_1 = eval("(cond (#t 1))");
    EXPECT_EQ(NUMBER, result_1->get_type());
    EXPECT_DOUBLE_EQ(1.0, result_1->as_number());
    const auto result_2 = eval("(cond (#f 1) (#t 2))");
    EXPECT_EQ(NUMBER, result_2->get_type());
    EXPECT_DOUBLE_EQ(2.0, result_2->as_number());
    const auto result_3 = eval("(cond (#f 1) (#f 2) (#t 3))");
    EXPECT_EQ(NUMBER, result_3->get_type());
    EXPECT_DOUBLE_EQ(3.0, result_3->as_number());
    // Test with no true conditions
    const auto result_nothing = eval("(cond (#f 1) (#f 2) (#f 3))");
    EXPECT_EQ(Expr::NOTHING, result_nothing);

    // Test with predicates
    const auto result_2_ = eval("(cond ((< 1 0) 1) ((> 1 0) 2))");
    EXPECT_DOUBLE_EQ(2.0, result_2_->as_number());
}

// IO operations tests
TEST_F(SchemePrimitivesTest, Display) {
    // Note: Testing display usually requires capturing output
    // This is a basic test that it doesn't crash
    EXPECT_NO_THROW(eval("(display \"hello\")"));
    EXPECT_NO_THROW(eval("(display 123)"));
    EXPECT_NO_THROW(eval("(display '(1 2 3))"));
}

TEST_F(SchemePrimitivesTest, Newline) {
    // Basic test that newline doesn't crash
    EXPECT_NO_THROW(eval("(newline)"));
}

TEST_F(SchemePrimitivesTest, Read) {
    // Testing read usually requires providing input
    // This depends on your implementation of the read function
    // You might need to mock or redirect stdin
}

// List operations tests
TEST_F(SchemePrimitivesTest, Cons) {
    const auto result = eval("(cons 1 2)");
    EXPECT_EQ(PAIR, result->get_type());
    const auto pair = result->as_pair();
    EXPECT_DOUBLE_EQ(1.0, pair->car()->as_number());
    EXPECT_DOUBLE_EQ(2.0, pair->cdr()->as_number());

    // Test building lists
    const auto list = eval("(cons 1 (cons 2 '()))");
    EXPECT_EQ(PAIR, list->get_type());
}

TEST_F(SchemePrimitivesTest, Car) {
    EXPECT_DOUBLE_EQ(1.0, eval("(car '(1 2 3))")->as_number());
    EXPECT_DOUBLE_EQ(1.0, eval("(car (cons 1 2))")->as_number());
}

TEST_F(SchemePrimitivesTest, Cdr) {
    const auto result = eval("(cdr '(1 2 3))");
    EXPECT_EQ(PAIR, result->get_type());
    EXPECT_DOUBLE_EQ(2.0, result->as_pair()->car()->as_number());

    EXPECT_DOUBLE_EQ(2.0, eval("(cdr (cons 1 2))")->as_number());
}

TEST_F(SchemePrimitivesTest, List) {
    const auto result = eval("(list 1 2 3)");
    EXPECT_EQ(PAIR, result->get_type());

    const auto pair = result->as_pair();
    EXPECT_DOUBLE_EQ(1.0, pair->car()->as_number());

    const auto second = pair->cdr()->as_pair();
    EXPECT_DOUBLE_EQ(2.0, second->car()->as_number());

    const auto third = second->cdr()->as_pair();
    EXPECT_DOUBLE_EQ(3.0, third->car()->as_number());
    EXPECT_TRUE(third->cdr()->is_nil());

    // Empty list
    EXPECT_TRUE(eval("(list)")->is_nil());
}

TEST_F(SchemePrimitivesTest, IsNull) {
    EXPECT_EQ(Expr::TRUE, eval("(null? '())"));
    EXPECT_EQ(Expr::FALSE, eval("(null? '(1 2 3))"));
    EXPECT_EQ(Expr::FALSE, eval("(null? 1)"));
    EXPECT_EQ(Expr::FALSE, eval("(null? #t)"));
}

// Complex integration tests
TEST_F(SchemePrimitivesTest, Factorial) {
    perform(R"(
        (define (factorial n)
            (if (= n 0)
                1
                (* n (factorial (- n 1)))))
    )");

    EXPECT_DOUBLE_EQ(1.0, eval("(factorial 0)")->as_number());
    EXPECT_DOUBLE_EQ(1.0, eval("(factorial 1)")->as_number());
    EXPECT_DOUBLE_EQ(2.0, eval("(factorial 2)")->as_number());
    EXPECT_DOUBLE_EQ(6.0, eval("(factorial 3)")->as_number());
    EXPECT_DOUBLE_EQ(24.0, eval("(factorial 4)")->as_number());
}

TEST_F(SchemePrimitivesTest, Fibonacci) {
    perform(R"(
        (define (fib n)
            (cond ((= n 0) 0)
                  ((= n 1) 1)
                  (else (+ (fib (- n 1)) (fib (- n 2))))))
    )");

    EXPECT_DOUBLE_EQ(0.0, eval("(fib 0)")->as_number());
    EXPECT_DOUBLE_EQ(1.0, eval("(fib 1)")->as_number());
    EXPECT_DOUBLE_EQ(1.0, eval("(fib 2)")->as_number());
    EXPECT_DOUBLE_EQ(2.0, eval("(fib 3)")->as_number());
    EXPECT_DOUBLE_EQ(3.0, eval("(fib 4)")->as_number());
}

TEST_F(SchemePrimitivesTest, DefineMapFunction) {
    perform("(define (double x) (* 2 x))");
    perform("(define (map proc list) (if (null? list) '() (cons (proc (car list)) (map proc (cdr list)))))");

    // This tests complex list manipulation
    const auto result = eval("(map double '(1 2 3 4))");
    EXPECT_EQ(PAIR, result->get_type());

    auto pair = result->as_pair();
    EXPECT_DOUBLE_EQ(2.0, pair->car()->as_number());

    pair = pair->cdr()->as_pair();
    EXPECT_DOUBLE_EQ(4.0, pair->car()->as_number());

    pair = pair->cdr()->as_pair();
    EXPECT_DOUBLE_EQ(6.0, pair->car()->as_number());

    pair = pair->cdr()->as_pair();
    EXPECT_DOUBLE_EQ(8.0, pair->car()->as_number());
    EXPECT_TRUE(pair->cdr()->is_nil());
}

// Error handling tests
TEST_F(SchemePrimitivesTest, ErrorConditions) {
    // These tests depend on your error handling implementation
    // You might want to test for exceptions or specific error values

    // Test wrong number of arguments
    // Test wrong types
    // Test unbound variables
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}