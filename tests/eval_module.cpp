#include <gtest/gtest.h>
#include <fstream>
#include <string>

#include "expr.h"
#include "context.h"
#include "error.h"
#include "parser.h"
#include "eval.h"

// Test fixture
class ModuleSystemTest : public ::testing::Test {
protected:
    void SetUp() override { ctx = make_root_context(); }
    void TearDown() override { ctx.reset(); }

    [[nodiscard]] shared_ptr<Expr> eval(const std::string& s) const {
        const auto exprs = parse(s);
        return ::eval(ctx, exprs);
    }


    void perform(const std::string& input) const
    {
        const auto exprs = parse(input);
        ::eval(ctx, exprs);
    }

    std::shared_ptr<Context> ctx;

    // Helpers to create temp .gl files
    static std::string write_file(const std::string& fname, const std::string& body) {
        std::ofstream out(fname);
        out << body;
        out.close();
        return fname;
    }
};

TEST_F(ModuleSystemTest, ProvideExplicitAndRequireOnlyInRename)
{
    // foo.gl exports a and c
    write_file("foo.gl",
        R"(
        (define a 10)
        (define b 20)
        (define c (+ a b))
        (provide a c)
        )");

    // Import only a and (rename c -> c1)
    perform(R"(
        (require (only-in "foo.gl" a (c c1)))
    )");

    const auto a = eval("a");
    const auto c1 = eval("c1");
    ASSERT_TRUE(a->is_number_int() && c1->is_number_int());
    EXPECT_EQ(integer(10), a->as_number_int());
    EXPECT_EQ(integer(30), c1->as_number_int());

    // b was not imported
    EXPECT_THROW(perform("b"), GlomError);
}

TEST_F(ModuleSystemTest, ProvideAllDefinedOutAndRequireAll)
{
    write_file("bar.gl",
        R"(
        (define x 1)
        (define y 2)
        (define z (+ x y))
        (provide (all-defined-out))
        )");

    perform(R"((require "bar.gl"))");

    EXPECT_EQ(integer(1), eval("x")->as_number_int());
    EXPECT_EQ(integer(2), eval("y")->as_number_int());
    EXPECT_EQ(integer(3), eval("z")->as_number_int());
}

TEST_F(ModuleSystemTest, LocalRequireScopesToLocalContext)
{
    write_file("loc.gl",
        R"(
        (define p 9)
        (provide p)
        )");

    // Bring p only into let scope via local-require
    perform(R"(
        (define have-local
          (let ()
            (local-require "loc.gl")
            p))
    )");

    // Inside let we captured p==9
    EXPECT_EQ(integer(9), eval("have-local")->as_number_int());

    // But in the root it should be unbound
    EXPECT_THROW(perform("p"), GlomError);
}
