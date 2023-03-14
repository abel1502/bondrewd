#include <bondrewd/internal/common.hpp>
#include <bondrewd/parse/parser.gen.hpp>
#include <bondrewd/ast/ast_nodes.gen.hpp>
#include <bondrewd/ast/visitor.hpp>

#include <iostream>
#include <typeinfo>

#include "demo_parser.hpp"


using namespace bondrewd;


namespace test_funcs {


struct DumpVisitor {
    unsigned depth = 0;

    DumpVisitor() = default;

    DumpVisitor(const DumpVisitor&) = delete;
    DumpVisitor(DumpVisitor&&) = default;
    DumpVisitor& operator=(const DumpVisitor&) = delete;
    DumpVisitor& operator=(DumpVisitor&&) = default;

    std::ostream& indent() const {
        for (unsigned i = 0; i < depth; ++i) {
            std::cout << "  ";
        }
        return std::cout;
    }

    void operator()(ast::abstract_ast_node auto& node) {
        indent() << typeid(node).name();
        visit(*this, node);
        indent() << "\n";
    }

    void operator()(ast::concrete_ast_node auto& node) {
        indent() << "(" << typeid(node).name() << ") {\n";
        ++depth;
        visit_recursive(*this, node);
        --depth;
        indent() << "}\n";
    }
};


void parser() {
    using namespace bondrewd::parse;

    std::cout << "Running parser test...\n";

    auto parser = Parser::from_string(
        "cartridge foo;\n"
        "\n"
        "func main(args: &[str]): int32 {"
        "    print(\"Bar!\");\n"
        "    0\n"
        "}\n"
    );

    // TODO: Unwrap results and throw exceptions inside `parse`
    auto result = parser.parse();

    if (!result) {
        std::cout << "Failed to parse.\n";
        return;
    }

    std::cout << "Parsed successfully.\n";

    const ast::cartridge &cartridge = **result;

    DumpVisitor()(cartridge);
}


} // namespace test_funcs
