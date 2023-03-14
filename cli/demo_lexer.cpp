#include <bondrewd/internal/common.hpp>
#include <bondrewd/lex/lexer.hpp>

#include <iostream>

#include "demo_lexer.hpp"


using namespace bondrewd;


namespace test_funcs {


void lexer() {
    using namespace bondrewd::lex;

    std::cout << "Running lexer test...\n";

    auto lexer = Lexer::from_string(
        "int main(type a, float *bcd) -> std::vector<something> {\n"
        "    *bcd = 1.2e3;  // not 12345 \n"
        "    *bcd = .5;  /* /* still not 12345 */*/ \n"
        "    *bcd = 12345.e+0;  # Hey!! \n"
        "    *bcd = 0xADC.DEpF; \n"
        "    return 0;\n"
        "}"
    );

    while (lexer) {
        lexer.advance();

        if (bondrewd::util::log_verbosity >= 2) {
            std::cout << ">";
            lexer.dump(std::cout) << "\n";
        }
    }

    lexer.dump(std::cout) << "\n";
}


} // namespace test_funcs
