#pragma once

#include <bondrewd/internal/common.hpp>
#include <bondrewd/parse/error.hpp>
#include <bondrewd/lex/lexer.hpp>

#include <concepts>


namespace bondrewd::parse {


template <typename T>
class ParserBase {
public:
    // #pragma region Static check
    // // Because a requires clause doesn't work, apparently...
    // static_assert(std::derived_from<T, ParserBase<T>>);
    // #pragma endregion Static check

    #pragma region Constructors
    ParserBase(lex::Lexer lexer) :
        lexer{std::move(lexer)} {}
    #pragma endregion Constructors

    #pragma region Service constructors
    ParserBase(const ParserBase &) = delete;
    ParserBase(ParserBase &&) = default;
    ParserBase &operator=(const ParserBase &) = delete;
    ParserBase &operator=(ParserBase &&) = default;
    #pragma endregion Service constructors

    #pragma region Factories
    static T from_stream(std::istream &input, std::string_view filename = "") {
        return T{lex::Lexer::from_stream(input, filename)};
    }

    static T from_string(std::string_view input, std::string_view filename = "") {
        return T{lex::Lexer::from_string(input, filename)};
    }

    static T from_file(std::filesystem::path filename) {
        return T{lex::Lexer::from_file(filename)};
    }
    #pragma endregion Factories

protected:
    #pragma region Fields
    lex::Lexer lexer;
    #pragma endregion Fields

    #pragma region Error
    // TODO: Error functions?

    [[noreturn]]
    void fatal_error(std::string_view msg) const {
        // TODO: Add SrcLocation info?

        throw SyntaxError{msg};
    }
    #pragma endregion Error

};


}  // namespace bondrewd::parse
