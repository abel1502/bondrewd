#pragma once

#include <bondrewd/internal/common.hpp>
#include <bondrewd/internal/arena.hpp>

#include <vector>
#include <variant>
#include <optional>
#include <memory>
#include <string_view>
#include <string>
#include <cstdint>
#include <concepts>
#include <tuple>
#include <utility>


namespace bondrewd::ast {


#pragma region Arena
extern util::Arena ast_arena;
#pragma endregion Arena


#pragma region Base classes
class _AbstractASTNode {};
class _ConcreteASTNode {};
#pragma endregion Base classes


#pragma region AST helper class
template <typename ... Alts>
class _AST : public _AbstractASTNode {
public:
    #pragma region Casting
    template <typename T>
    constexpr bool is() const {
        return std::holds_alternative<T>(value);
    }

    template <typename T>
    constexpr T &as() {
        return std::get<T>(value);
    }

    template <typename T>
    constexpr const T &as() const {
        return std::get<T>(value);
    }
    #pragma endregion Casting
    
    #pragma region Service constructors
    _AST(const _AST &) = delete;
    _AST(_AST &&) = default;
    _AST &operator=(const _AST &) = delete;
    _AST &operator=(_AST &&) = default;
    #pragma endregion Service constructors

    #pragma region Fields
    std::variant<Alts...> value;
    #pragma endregion Fields

protected:
    #pragma region Constructors
    _AST() = default;

    _AST(auto &&value) :
        value{std::forward<decltype(value)>(value)} {}
    #pragma endregion Constructors

};
#pragma endregion AST helper class


#pragma region Concepts
// Note: these are imperfect, in that AST<...> and _AbstractASTNode/_ConcreteASTNode
// themselves satisfy these. However, you shouldn't face instances of these classes
// in reality, so it's not really a problem.

template <typename T>
concept abstract_ast_node = std::derived_from<T, _AbstractASTNode>;

template <typename T>
concept concrete_ast_node = std::derived_from<T, _ConcreteASTNode>;
#pragma endregion Concepts


#pragma region Convenience aliases
#pragma region field
template <typename T>
using field = util::arena_ptr<T, &ast_arena>;

template <typename T, typename ... As>
field<T> make_field(As &&... args) {
    return util::make_arena_ptr<T, &ast_arena>(std::forward<As>(args)...);
}
#pragma endregion field

#pragma region sequence
template <typename T>
using sequence = util::arena_ptr<std::vector<T>, &ast_arena>;

template <typename T, typename ... As>
sequence<T> make_sequence(As &&... args) {
    return util::make_arena_ptr<std::vector<T>, &ast_arena>{std::forward<As>(args)...};
}
#pragma endregion sequence

#pragma region maybe
template <typename T>
using maybe = field<T>;

template <typename T, typename ... As>
maybe<T> make_maybe(As &&... args) {
    return make_field<T>(std::forward<As>(args)...);
}

template <typename T>
maybe<T> make_maybe(std::nullopt_t) {
    return nullptr;
}

template <typename T>
maybe<T> make_maybe(std::nullptr_t) {
    return nullptr;
}

template <typename T>
maybe<T> make_maybe() {
    return nullptr;
}
#pragma endregion maybe
#pragma endregion Convenience aliases


#pragma region ASDL basic types
// Types that match the C++ built-in ones aren't redefined here

// Identifiers are stored as views to the buffered source code
using identifier = std::string_view;

using string = std::string;

using string_view = std::string_view;

// TODO: Temporary! Will be replaced with a compile-time object type
// Monostate represents unit for now
using constant = std::variant<int64_t, double, std::string, std::monostate>;
#pragma endregion ASDL basic types


}  // namespace bondrewd::ast
