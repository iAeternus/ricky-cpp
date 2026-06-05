#include "test_expr.hpp"
#include "expr.hpp"
#include "ricky_test.hpp"

namespace my::test::test_expr {

void it_works() {
    math::Expr expr = "3 + 4*2/(1-5)^2"_expr;
    math::Expr expr2 = "2^3^2"_expr;
    math::Expr expr3 = "-5%3"_expr;
    math::Expr expr4 = "3.5 + 4.2*(2-5.1)/2"_expr;

    Assertions::assert_equals("[3,+,4,*,2,/,(,1,-,5,),^,2]"_cs, expr.to_string());
    Assertions::assert_equals("[2,^,3,^,2]"_cs, expr2.to_string());
    Assertions::assert_equals("[u-,5,%,3]"_cs, expr3.to_string());
    Assertions::assert_equals("[3.5,+,4.2,*,(,2,-,5.1,),/,2]"_cs, expr4.to_string());

    Assertions::assert_equals("[3,4,2,*,1,5,-,2,^,/,+]"_cs, expr.to_post().to_string());
    Assertions::assert_equals("[2,3,2,^,^]"_cs, expr2.to_post().to_string());
    Assertions::assert_equals("[5,u-,3,%]"_cs, expr3.to_post().to_string());
    Assertions::assert_equals("[3.5,4.2,2,5.1,-,*,2,/,+]"_cs, expr4.to_post().to_string());

    Assertions::assert_equals(3 + 4 * 2 / pow(1 - 5, 2), expr.eval());
    Assertions::assert_equals(pow(2, pow(3, 2)), expr2.eval());
    Assertions::assert_equals(fmod(-5, 3), expr3.eval());
    Assertions::assert_equals(3.5 + 4.2 * (2 - 5.1) / 2, expr4.eval());
}

GROUP_NAME("test_expr")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(it_works))

} // namespace my::test::test_expr
