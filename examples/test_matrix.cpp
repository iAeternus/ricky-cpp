/**
 * @brief
 * @author Ricky
 * @date 2025/10/31
 * @version 1.0
 */
#include "matrix.hpp"
#include "printer.hpp"

int main() {
    my::math::Matrix<double> P1 = {
        {-1, -2, 1},
        {2, 1, -2},
        {4, 0, 0},
    };

    my::math::Matrix<double> P2 = {
        {0, -1, 1},
        {0, 3, 0},
        {1, 0, 0},
    };

    my::io::println(P2.inv());
    my::io::println(P1 * P2.inv());
    return 0;
}
