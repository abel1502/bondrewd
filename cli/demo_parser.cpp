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

    DumpVisitor(const DumpVisitor &) = delete;
    DumpVisitor(DumpVisitor &&) = default;
    DumpVisitor& operator=(const DumpVisitor &) = delete;
    DumpVisitor& operator=(DumpVisitor &&) = default;

    std::ostream& indent() const {
        for (unsigned i = 0; i < depth; ++i) {
            std::cout << "  ";
        }
        return std::cout;
    }

    static constexpr std::string_view _remove_prefix(std::string_view str, std::string_view prefix) {
        if (str.starts_with(prefix)) {
            str.remove_prefix(prefix.size());
        }

        return str;
    }

    template <typename T>
    static constexpr std::string_view get_ast_name() {
        std::string_view result = std::string_view(typeid(T).name());

        return _remove_prefix(result, "class bondrewd::ast::nodes::");
    }

    void operator()(ast::abstract_ast_node auto &node) {
        indent() << get_ast_name<decltype(node)>();
        visit<void>(*this, node);
    }

    void operator()(ast::concrete_ast_node auto &node) {
        std::cout << " (" << get_ast_name<decltype(node)>() << ") {\n";
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
        "func main(/* args: &[str] */): int32 => {\n"
        "    // print(\"Bar!\");\n"
        "    0\n"
        "};\n"
    );

    ast::field<ast::cartridge> result{};

    try {
        result = parser.parse();
    } catch (const parse::SyntaxError &e) {
        std::cout << "[Error] " << e.what() << "\n";
        return;
    }

    std::cout << "Parsed successfully.\n";

    ast::cartridge &cartridge = *result;

    DumpVisitor()(cartridge);
}


} // namespace test_funcs
