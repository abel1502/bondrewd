#pragma once

#include <bondrewd/internal/common.hpp>
#include <bondrewd/ast/ast.hpp>

#include <concepts>
#include <utility>
#include <exception>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <concepts>
#include <fmt/core.h>


namespace bondrewd::ast {


#pragma region overloaded
template<class ... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class ... Ts> overloaded(Ts...) -> overloaded<Ts...>;
#pragma endregion overloaded


#pragma region for_each
template <typename T>
void for_each(auto &&func, T &&tuple) {
    std::apply([&](auto &&... args) {
        (std::invoke(func, std::forward<decltype(args)>(args)), ...);
    }, std::forward<T>(tuple));
}
#pragma endregion for_each


#pragma region visit
decltype(auto) visit(auto &&func, abstract_ast_node auto &&node) {
    return std::visit(std::forward<decltype(func)>(func), node.value);
}

template<typename R>
R visit(auto &&func, abstract_ast_node auto &&node) {
    return std::visit<R>(std::forward<decltype(func)>(func), node.value);
}
#pragma endregion visit


#pragma region visit_recursive
decltype(auto) visit_recursive(auto &&func, concrete_ast_node auto &&node) {
    for_each([&](auto &child) {
        using child_type = std::decay_t<decltype(child)>;

        // DBG("Visiting %s", typeid(child).name());

        if constexpr (is_sequence<child_type>) {
            // DBG("Sequence");
            if constexpr (std::invocable<decltype(func), typename child_type::element_type::value_type &>) {
                for (auto& child_item : *child) {
                    func(child_item);
                }
            }
        } else if constexpr (is_field<child_type> || is_maybe<child_type>) {
            // DBG("Field");
            if constexpr (std::invocable<decltype(func), typename child_type::element_type &>) {
                if (child) {
                    func(*child);
                }
            }
        } else {
            // DBG("Skipping");
        }
    }, node.get_fields_tuple());
}
#pragma endregion visit_recursive


}  // namespace bondrewd::ast
