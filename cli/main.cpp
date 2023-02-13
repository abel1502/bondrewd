#include <bondrewd/internal/common.hpp>
#include <bondrewd/lex/lexer.hpp>

#include <boost/program_options.hpp>
#include <iostream>


#pragma region Helpers
namespace {


auto parse_args(int argc, char **argv) {
    namespace prog_opts = boost::program_options;

    prog_opts::options_description desc{"Allowed options"};
    desc.add_options()
        ("help", "produce help message")
        ("verbosity", prog_opts::value<int>()->default_value(0)->implicit_value(1), "set verbosity level")
        ("test", prog_opts::bool_switch(), "run a debug test")
    ;

    prog_opts::variables_map args{};
    try {
        prog_opts::store(
            prog_opts::command_line_parser(argc, argv)
                .options(desc).run(),
            args
        );
        prog_opts::notify(args);
    } catch (const prog_opts::error &e) {
        std::cerr << "Error: " << e.what() << "\n";
        std::cout << desc << "\n";
        exit(1);
    }

    if (args.count("help")) {
        std::cout << desc << "\n";
        exit(0);
    }

    return args;
}


void run_test() {
    using namespace bondrewd::lex;

    std::cout << "Running debug test...\n";

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
    std::cout << "Done.\n";
}


}  // namespace
#pragma endregion Helpers





int main(int argc, char **argv) {
    auto args = parse_args(argc, argv);

    bondrewd::util::log_verbosity = args["verbosity"].as<int>();

    if (args.count("test")) {
        run_test();
    }

    return 0;
}
