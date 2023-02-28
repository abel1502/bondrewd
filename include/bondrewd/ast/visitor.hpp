#pragma once

#include <bondrewd/internal/common.hpp>
#include <bondrewd/ast/ast.hpp>

#include <concepts>
#include <utility>
#include <exception>
#include <stdexcept>
#include <fmt/core.h>


namespace bondrewd::ast {


template<class ... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class ... Ts> overloaded(Ts...) -> overloaded<Ts...>;


template <typename V, typename T>
concept has_concrete_visit = requires(V visitor, T node) {
    visitor.visit(node);
};


template <typename T, typename R = void>
requires std::derived_from<T, VisitorBase<T>>
class VisitorBase {
public:
    R visit(auto abstract_ast_node &node) {
        std::visit(node.value, *self());
    }

    R visit(auto concrete_ast_node &node) {
        throw std::runtime_error(fmt::format(
            "Visitor {} does not support concrete AST node {}",
            typeid(*self()).name(), typeid(node).name()
        ));
    }

    R operator()(auto &node) {
        visit(node);
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
                    self()->visit(child);
                }
                return;
            }

            if (child) {
                self()->visit(child);
            }
        }, node.get_fields_tuple());
    }
};


}  // namespace bondrewd::ast
