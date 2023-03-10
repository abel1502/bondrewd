#pragma once

#include <bondrewd/internal/common.hpp>
#include <bondrewd/lex/scanner.hpp>
#include <bondrewd/lex/token.hpp>
#include <bondrewd/lex/error.hpp>

#include <cctype>
#include <fmt/core.h>
#include <ranges>
#include <cmath>
#include <cstdint>


namespace bondrewd::lex {


class Tokenizer {
public:
    #pragma region Constructors
    Tokenizer(Scanner scanner) :
        scanner{std::move(scanner)} {}
    #pragma endregion Constructors

    #pragma region Service constructors
    Tokenizer(const Tokenizer &) = delete;
    Tokenizer(Tokenizer &&) = default;
    Tokenizer &operator=(const Tokenizer &) = delete;
    Tokenizer &operator=(Tokenizer &&) = default;
    #pragma endregion Service constructors

    #pragma region Factories
    static Tokenizer from_stream(std::istream &input, std::string_view filename = "") {
        return Tokenizer{Scanner::from_stream(input, filename)};
    }

    static Tokenizer from_string(std::string_view input, std::string_view filename = "") {
        return Tokenizer{Scanner::from_string(input, filename)};
    }

    static Tokenizer from_file(std::filesystem::path filename) {
        return Tokenizer{Scanner::from_file(filename)};
    }
    #pragma endregion Factories

    #pragma region Interface
    /**
     * Get the next token from the input stream.
     *
     * Note: caching is expected to be done by the caller.
     *
     * @throws LexicalError if the next token is invalid.
     */
    Token get_token();
    #pragma endregion Interface

protected:
    #pragma region Fields
    Scanner scanner;
    Token token{Token::endmarker(SrcLocation{}, "")};
    #pragma endregion Fields

    #pragma region Private constants
    static const std::unordered_map<char, char> simple_escapes;
    static constexpr unsigned MAX_NUMBER_LENGTH = 128;
    #pragma endregion Private constants

    #pragma region Character classification
    static bool is_name_char(int c) {
        return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || is_digit(c) || c == '_';
    }

    static bool is_digit(int c) {
        return '0' <= c && c <= '9';
    }

    static bool is_hex_digit(int c) {
        return ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F') || is_digit(c);
    }

    static int evaluate_digit(int c) {
        if ('0' <= c && c <= '9') {
            return c - '0';
        }
        if ('a' <= c && c <= 'z') {
            return c - 'a' + 10;
        }
        if ('A' <= c && c <= 'Z') {
            return c - 'A' + 10;
        }

        return -1;
    }

    static int evaluate_digit(int c, unsigned base) {
        assert(2 <= base && base <= 36);

        unsigned result = evaluate_digit(c);

        if (result >= base) {
            return -1;
        }

        return result;
    }

    unsigned evaluate_digit_checked(int c, unsigned base) {
        return evaluate_digit_checked(c, base, scanner.get_loc());
    }

    unsigned evaluate_digit_checked(int c, unsigned base, const SrcLocation &loc) {
        assert(2 <= base && base <= 36);

        int result = evaluate_digit(c);

        if (result < 0) {
            error(fmt::format("Invalid digit '{}'", (char)c), loc);
        }

        if ((unsigned)result >= base) {
            error(fmt::format("Wrong digit '{}' for base {}", (char)c, base), loc);
        }

        return (unsigned)result;
    }
    #pragma endregion Character classification

    #pragma region Number parsing
    int64_t extract_integer(std::string_view digits, unsigned base, const SrcLocation &loc);

    double extract_float(std::string_view integral_part,
                         std::string_view fractional_part,
                         std::string_view exponent_part,
                         int exponent_sign,
                         unsigned base,
                         const SrcLocation &loc);
    #pragma endregion Number parsing

    #pragma region Tokenization subroutines
    void parse_number();

    void parse_name_or_keyword();

    void parse_name();

    /// Returns true if a token has been parsed, false if a comment has been consumed instead
    bool parse_other();

    void parse_string(std::string_view start_quote);

    void parse_line_comment();

    void parse_block_comment();
    #pragma endregion Tokenization subroutines

    #pragma region Error
    [[noreturn]]
    void error(std::string message, const SrcLocation &loc) {
        // TODO: Save or use the loc parameter
        (void)loc;

        throw LexicalError{std::move(message)};
    }
    #pragma endregion Error

};


}  // namespace bondrewd::lex
