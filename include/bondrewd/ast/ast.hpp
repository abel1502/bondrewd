#pragma once

#include <bondrewd/internal/common.hpp>

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
    T &as() {
        return std::get<T>(value);
    }

    template <typename T>
    const T &as() const {
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
template <typename T>
using field = std::unique_ptr<T>;


template <typename T>
using sequence = std::vector<T>;


// TODO: special helper for optional?
#pragma endregion Convenience aliases


#pragma region ASDL basic types
// Types that match the C++ built-in ones aren't redefined here

// Identifiers are stored as views to the buffered source code
using identifier = std::string_view;

using string = std::string;

using string_view = std::string_view;

// TODO: Temporary! Will be replaced with a compile-time object type
using constant = std::variant<int64_t, double, std::string>;
#pragma endregion ASDL basic types


}  // namespace bondrewd::ast
