#include "Printer.hpp"
#include "Log.hpp"
#include "Exception.hpp"

int main() {
    my::io::println(__cplusplus);

    // my::runtime_exception("some thing cause this runtime exception");
    my::Exception ex(my::ExceptionType::RuntimeException, "some thing cause this runtime exception");
    ex.log();
}