#include <bondrewd/internal/common.hpp>

#include <boost/program_options.hpp>
#include <iostream>

#include "demo_lexer.hpp"
#include "demo_parser.hpp"


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
    // test_funcs::lexer();
    test_funcs::parser();

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
