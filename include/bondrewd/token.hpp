#pragma once

#include <cstddef>
#include <utility>
#include <variant>
#include <cstdint>

#include <bondrewd/tokens.generated.hpp>
#include <bondrewd/src_location.hpp>


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
    static Token endmarker(SrcLocation location) {
        return Token{EndmarkerValue{}, location};
    }

    static Token name(std::string_view value, SrcLocation location) {
        return Token{NameValue{value}, location};
    }

    static Token number(int64_t value, SrcLocation location) {
        return Token{NumberValue{value}, location};
    }

    static Token number(double value, SrcLocation location) {
        return Token{NumberValue{value}, location};
    }

    static Token string(std::string_view value, char quote_char, SrcLocation location) {
        return Token{StringValue{value, quote_char}, location};
    }

    static Token keyword(HardKeyword value, SrcLocation location) {
        return Token{KeywordValue{value}, location};
    }

    static Token punct(Punct value, SrcLocation location) {
        return Token{PunctValue{value}, location};
    }
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
    };

    struct PunctValue {
        Punct value;
    };

public:
    #pragma endregion Value structs

    #pragma region Getters
    TokenType get_type() const noexcept {
        return type;
    }

    const SrcLocation &get_location() const noexcept {
        return location;
    }

    const EndmarkerValue &get_endmarker() const noexcept {
        return std::get<EndmarkerValue>(value);
    }

    EndmarkerValue &get_endmarker() noexcept {
        return std::get<EndmarkerValue>(value);
    }

    const NameValue &get_name() const noexcept {
        return std::get<NameValue>(value);
    }

    NameValue &get_name() noexcept {
        return std::get<NameValue>(value);
    }

    const NumberValue &get_number() const noexcept {
        return std::get<NumberValue>(value);
    }

    NumberValue &get_number() noexcept {
        return std::get<NumberValue>(value);
    }

    const StringValue &get_string() const noexcept {
        return std::get<StringValue>(value);
    }

    StringValue &get_string() noexcept {
        return std::get<StringValue>(value);
    }

    const KeywordValue &get_keyword() const noexcept {
        return std::get<KeywordValue>(value);
    }

    KeywordValue &get_keyword() noexcept {
        return std::get<KeywordValue>(value);
    }

    const PunctValue &get_punct() const noexcept {
        return std::get<PunctValue>(value);
    }

    PunctValue &get_punct() noexcept {
        return std::get<PunctValue>(value);
    }
    #pragma endregion Getters

    #pragma region Predicates
    bool is_endmarker() const noexcept {
        return type == TokenType::endmarker;
    }

    bool is_name() const noexcept {
        return type == TokenType::name;
    }

    bool is_number() const noexcept {
        return type == TokenType::number;
    }

    bool is_string() const noexcept {
        return type == TokenType::string;
    }

    bool is_keyword() const noexcept {
        return type == TokenType::keyword;
    }

    bool is_punct() const noexcept {
        return type == TokenType::punct;
    }
    #pragma endregion Predicates

protected:
    #pragma region Fields
    TokenType type;
    SrcLocation location{};
    std::variant<
        EndmarkerValue,
        NameValue,
        NumberValue,
        StringValue,
        KeywordValue,
        PunctValue
    > value;
    #pragma endregion Fields

    #pragma region Protected constructors
    Token(EndmarkerValue value, SrcLocation location) :
        type{TokenType::endmarker}, value{value}, location{location} {}

    Token(NameValue value, SrcLocation location) :
        type{TokenType::name}, value{value}, location{location} {}
    
    Token(NumberValue value, SrcLocation location) :
        type{TokenType::number}, value{value}, location{location} {}
    
    Token(StringValue value, SrcLocation location) :
        type{TokenType::string}, value{value}, location{location} {}
    
    Token(KeywordValue value, SrcLocation location) :
        type{TokenType::keyword}, value{value}, location{location} {}
    
    Token(PunctValue value, SrcLocation location) :
        type{TokenType::punct}, value{value}, location{location} {}
    #pragma endregion Protected constructors

};


}  // namespace bondrewd::lex
