#ifndef TEST_EXPRESSION_HPP
#define TEST_EXPRESSION_HPP

#include "ricky_test.hpp"
#include "Expression.hpp"

namespace my::test::test_expression {

auto it_works = []() {
    math::Expression expr = "3 + 4*2/(1-5)^2"_expr;
    math::Expression expr2 = "2^3^2"_expr;
    math::Expression expr3 = "-5%3"_expr;
    math::Expression expr4 = "3.5 + 4.2*(2-5.1)/2"_expr;

    Assertions::assertEquals("[3,+,4,*,2,/,(,1,-,5,),^,2]"_cs, expr.__str__());
    Assertions::assertEquals("[2,^,3,^,2]"_cs, expr2.__str__());
    Assertions::assertEquals("[u-,5,%,3]"_cs, expr3.__str__());
    Assertions::assertEquals("[3.5,+,4.2,*,(,2,-,5.1,),/,2]"_cs, expr4.__str__());

    Assertions::assertEquals("[3,4,2,*,1,5,-,2,^,/,+]"_cs, expr.toPostfix().__str__());
    Assertions::assertEquals("[2,3,2,^,^]"_cs, expr2.toPostfix().__str__());
    Assertions::assertEquals("[5,u-,3,%]"_cs, expr3.toPostfix().__str__());
    Assertions::assertEquals("[3.5,4.2,2,5.1,-,*,2,/,+]"_cs, expr4.toPostfix().__str__());

    Assertions::assertEquals(3 + 4 * 2 / pow(1 - 5, 2), expr.eval());
    Assertions::assertEquals(pow(2, pow(3, 2)), expr2.eval());
    Assertions::assertEquals(fmod(-5, 3), expr3.eval());
    Assertions::assertEquals(3.5 + 4.2 * (2 - 5.1) / 2, expr4.eval());
};

void test_expression() {
    UnitTestGroup group{"test_expression"};

    group.addTest("it_works", it_works);

    group.startAll();
}

} // namespace my::test::test_expression

#endif // TEST_EXPRESSION_HPP