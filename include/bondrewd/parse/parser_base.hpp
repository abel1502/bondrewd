#pragma once

#include <bondrewd/internal/common.hpp>
#include <bondrewd/parse/error.hpp>
#include <bondrewd/lex/lexer.hpp>

#include <concepts>


namespace bondrewd::parse {


template <typename T>
requires std::derived_from<T, ParserBase<T>>
class ParserBase {
public:
    #pragma region Constructors
    ParserBase(Lexer lexer) :
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
        return T{Lexer::from_stream(input, filename)};
    }

    static T from_string(std::string_view input, std::string_view filename = "") {
        return T{Lexer::from_string(input, filename)};
    }

    static T from_file(std::filesystem::path filename) {
        return T{Lexer::from_file(filename)};
    }
    #pragma endregion Factories

protected:
    #pragma region Fields
    Lexer lexer;
    #pragma endregion Fields

    #pragma region Helpers
    bool lookahead(bool positive, auto rule_func) {
        auto guard = lexer.lookahead();

        // TODO: Pass this?
        return positive == (bool)rule_func();
    }

    template <typename U>
    U forced(U result) {
        if (!result) {
            // TODO: More info?
            fatal_error("Forced rule failed");
        }

        return std::move(result);
    }
    #pragma endregion Helpers

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
