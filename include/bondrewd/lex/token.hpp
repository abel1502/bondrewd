#pragma once

#include <bondrewd/internal/common.hpp>
#include <bondrewd/lex/src_location.hpp>
#include <bondrewd/lex/tokens.generated.hpp>

#include <variant>
#include <cstdint>


namespace bondrewd::lex {


enum class TokenType {
    endmarker,
    name,
    number,
    string,
    // newline,
    keyword,
    punct,
};


class Token {
public:
    #pragma region Factories
    #define TOKEN_FACTORY_(VALUE_STRUCT, TYPE, PARAMS, PARAMS_FOR_STRUCT) \
        static inline Token TYPE(PARAMS SrcLocation location, std::string_view source) { \
            return Token{VALUE_STRUCT{PARAMS_FOR_STRUCT}, std::move(location), std::move(source)}; \
        }

    TOKEN_FACTORY_(EndmarkerValue, endmarker, MACRO_PASS(), MACRO_PASS())
    TOKEN_FACTORY_(NameValue, name, MACRO_PASS(std::string_view value,), MACRO_PASS(std::move(value)))
    TOKEN_FACTORY_(NumberValue, number, MACRO_PASS(int64_t value,), MACRO_PASS(value))
    TOKEN_FACTORY_(NumberValue, number, MACRO_PASS(double value,), MACRO_PASS(value))
    TOKEN_FACTORY_(StringValue, string, MACRO_PASS(std::string_view value, char quote_char,), MACRO_PASS(std::move(value), quote_char))
    TOKEN_FACTORY_(KeywordValue, keyword, MACRO_PASS(HardKeyword value,), MACRO_PASS(value))
    TOKEN_FACTORY_(PunctValue, punct, MACRO_PASS(Punct value,), MACRO_PASS(value))

    #undef TOKEN_FACTORY_
    #pragma endregion Factories

    #pragma region Service constructors
    Token(const Token &) = default;
    Token(Token &&) = default;
    Token &operator=(const Token &) = default;
    Token &operator=(Token &&) = default;
    #pragma endregion Service constructors

    #pragma region Value structs
protected:
    struct EndmarkerValue {};

    struct NameValue {
        std::string_view value;
    };

    struct NumberValue {
        std::variant<
            int64_t,
            double
        > value;

        constexpr bool is_int() const noexcept {
            return std::holds_alternative<int64_t>(value);
        }

        constexpr bool is_float() const noexcept {
            return std::holds_alternative<double>(value);
        }
    };

    struct StringValue {
        std::string_view value;
        char quote_char;
    };

    struct KeywordValue {
        HardKeyword value;

        const char *to_string() const noexcept {
            return keyword_to_string(value);
        }
    };

    struct PunctValue {
        Punct value;

        const char *to_string() const noexcept {
            return punct_to_string(value);
        }
    };

public:
    #pragma endregion Value structs

    #pragma region Getters
    TokenType get_type() const noexcept {
        return type;
    }

    #define TOKEN_VALUE_GETTER_(VALUE_STRUCT, TYPE) \
        const VALUE_STRUCT &get_##TYPE() const { \
            return std::get<VALUE_STRUCT>(value); \
        } \
        \
        VALUE_STRUCT &get_##TYPE() { \
            return std::get<VALUE_STRUCT>(value); \
        }
    
    TOKEN_VALUE_GETTER_(EndmarkerValue, endmarker)
    TOKEN_VALUE_GETTER_(NameValue, name)
    TOKEN_VALUE_GETTER_(NumberValue, number)
    TOKEN_VALUE_GETTER_(StringValue, string)
    TOKEN_VALUE_GETTER_(KeywordValue, keyword)
    TOKEN_VALUE_GETTER_(PunctValue, punct)
    
    #undef TOKEN_VALUE_GETTER_

    const SrcLocation &get_location() const noexcept {
        return location;
    }

    const std::string_view &get_source() const noexcept {
        return source;
    }
    #pragma endregion Getters

    #pragma region Predicates
    #define TOKEN_TYPE_PREDICATE_(TYPE) \
        bool is_##TYPE() const noexcept { \
            return type == TokenType::TYPE; \
        }
    
    TOKEN_TYPE_PREDICATE_(endmarker)
    TOKEN_TYPE_PREDICATE_(name)
    TOKEN_TYPE_PREDICATE_(number)
    TOKEN_TYPE_PREDICATE_(string)
    TOKEN_TYPE_PREDICATE_(keyword)
    TOKEN_TYPE_PREDICATE_(punct)

    #undef TOKEN_TYPE_PREDICATE_
    #pragma endregion Predicates

protected:
    #pragma region Fields
    TokenType type;
    std::variant<
        EndmarkerValue,
        NameValue,
        NumberValue,
        StringValue,
        KeywordValue,
        PunctValue
    > value;
    SrcLocation location{};
    std::string_view source{};
    #pragma endregion Fields

    #pragma region Protected constructors
    #define TOKEN_CTOR_(VALUE_STRUCT, TYPE) \
        inline Token(VALUE_STRUCT value, SrcLocation location, std::string_view source) : \
            type{TokenType::TYPE}, \
            value{std::move(value)}, \
            location{std::move(location)}, \
            source{std::move(source)} {}

    TOKEN_CTOR_(EndmarkerValue, endmarker)
    TOKEN_CTOR_(NameValue, name)
    TOKEN_CTOR_(NumberValue, number)
    TOKEN_CTOR_(StringValue, string)
    TOKEN_CTOR_(KeywordValue, keyword)
    TOKEN_CTOR_(PunctValue, punct)
    
    #undef TOKEN_CTOR_
    #pragma endregion Protected constructors

};


}  // namespace bondrewd::lex
