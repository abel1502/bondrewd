#pragma once

#include <bondrewd/internal/common.hpp>
#include <bondrewd/ast/ast.hpp>

#include <concepts>
#include <utility>
#include <fmt/core.h>


namespace bondrewd::ast {


template<class ... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class ... Ts> overloaded(Ts...) -> overloaded<Ts...>;


template <typename V, typename T>
concept has_concrete_visit = requires(V visitor, T node) {
    visitor.visit(node);
};


template <typename T>
requires std::derived_from<T, VisitorBase<T>>
class VisitorBase {
public:
    void visit(auto abstract_ast_node &node) {
        node.accept(*self());
    }

    void visit(auto concrete_ast_node &node) {
        fmt::format("Visitor {} does not support concrete AST node {}",
                    typeid(*self()).name(), typeid(node).name());
    }

protected:
    T *self() {
        return static_cast<T *>(this);
    }

    const T *self() const {
        return static_cast<const T *>(this);
    }
};


template <typename T>
class RecursiveVisitorBase : public VisitorBase<T> {
public:
    void visit(auto concrete_ast_node &node) {
        std::apply([&](auto &child) {
            if constexpr (util::specialization_of<decltype(child), sequence>) {
                for (auto &child : child) {
                    child.accept(*self());
                }
                return;
            }

            if (child) {
                child->accept(*self());
            }
        }, node.all_fields_tuple());
    }
};


}  // namespace bondrewd::ast
