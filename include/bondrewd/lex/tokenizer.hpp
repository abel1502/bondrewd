#pragma once

#include <bondrewd/internal/common.hpp>
#include <bondrewd/lex/scanner.hpp>
#include <bondrewd/lex/token.hpp>

#include <cctype>
#include <exception>
#include <fmt/core.h>
#include <ranges>
#include <cmath>


namespace bondrewd::lex {


// TODO: Add location info!
// (Either to the error or to the lexer state)
DECLARE_ERROR(LexicalError, std::runtime_error);


JUST_DECLARE(class, Tokenizer) {
public:
    #pragma region Constructors
    Tokenizer(Scanner source) :
        scanner{std::move(source)} {}
    #pragma endregion Constructors

    #pragma region Service constructors
    Tokenizer(const Tokenizer &) = delete;
    Tokenizer(Tokenizer &&) = delete;
    Tokenizer &operator=(const Tokenizer &) = delete;
    Tokenizer &operator=(Tokenizer &&) = delete;
    #pragma endregion Service constructors

    #pragma region Interface
    /**
     * Get the next token from the input stream.
     *
     * Note: caching is expected to be done by the caller.
     *
     * @throws LexicalError if the next token is invalid.
     */
    Token get_token() {
        // Loop needed because of how comments are handled
        while (scanner) {
            scanner.skip_space();

            if (is_digit(scanner.cur())) {
                parse_number();
                return token;
            }

            if (is_name_char(scanner.cur())) {
                parse_name_or_keyword();
                return token;
            }

            if (parse_other()) {
                return token;
            }
        }

        token = Token::endmarker(scanner.get_loc(), "");
        return token;
    }
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
        return 0 <= c && c <= 9;
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
    unsigned long long extract_integer(std::string_view digits, unsigned base, const SrcLocation &loc) {
        // Digits are assumed to be correct

        unsigned long long result = 0;

        for (auto digit : digits) {
            // TODO: Maybe make MSVC-compatible
            if (__builtin_mul_overflow(result, base, &result))
                error("Integer literal too large", loc);
            if (__builtin_add_overflow(result, evaluate_digit(digit, base), &result))
                error("Integer literal too large", loc);
        }

        return result;
    }

    double extract_float(std::string_view integral_part,
                         std::string_view fractional_part,
                         std::string_view exponent_part,
                         int exponent_sign,
                         unsigned base,
                         const SrcLocation &loc) {
        // Digits are assumed to be correct

        double result = 0;

        for (auto digit : integral_part) {
            result *= base;
            result += evaluate_digit(digit, base);
        }

        double fraction = 0;
        for (auto digit : std::ranges::reverse_view(fractional_part)) {
            fraction += evaluate_digit(digit, base);
            fraction /= base;
        }

        result += fraction;

        if (exponent_sign != 0) {
            unsigned long long exponent = extract_integer(exponent_part, base, loc);

            result *= std::exp2(exponent_sign * exponent);
        }

        return result;
    }

    #pragma endregion Number parsing

    #pragma region Tokenization subroutines
    void parse_number() {
        auto start_pos = scanner.tell();

        unsigned base = 10;

        // Note: We don't parse sign here, because it will be treated as an unary operator instead

        if (scanner.cur() == '0') {
            switch (scanner.peek_next()) {
            case 'B':
            case 'b':
                base = 2;
                scanner.advance(2);
                break;

            case 'O':
            case 'o':
                base = 8;
                scanner.advance(2);
                break;

            case 'X':
            case 'x':
                base = 16;
                scanner.advance(2);
                break;

            default:
                break;
            }
        }

        std::string_view integral_part = scanner.read_while(
            [base](int c) { return evaluate_digit(c, base) >= 0; }
        );

        bool is_integer = true;

        std::string_view fraction_part{};
        std::string_view exp_part{};
        int exp_sign = 0;

        if (scanner.cur() == '.' \
            && (!integral_part.empty() || evaluate_digit(scanner.peek_next(), base) >= 0)) {

            scanner.advance();
            is_integer = false;

            fraction_part = scanner.read_while(
                [base](int c) { return evaluate_digit(c, base) >= 0; }
            );
        }

        if (base == 10 \
            && (!fraction_part.empty() || !integral_part.empty()) \
            && (scanner.cur() == 'E' || scanner.cur() == 'e')) {

            scanner.advance();
            is_integer = false;
            exp_sign = 1;

            switch (scanner.cur()) {
            case '+':
                scanner.advance();
                break;

            case '-':
                scanner.advance();
                exp_sign = -1;
                break;

            default:
                break;
            }

            // Fine since we're always in base 10 here
            exp_part = scanner.read_while<is_digit>();
        }

        if (is_integer && integral_part.empty()) {
            error("Invalid integer literal", start_pos.loc);
        }

        if (is_name_char(scanner.cur())) {
            error(fmt::format("Garbage '{}' after number", (char)scanner.cur()), scanner.get_loc());
        }

        if (integral_part.size() >= MAX_NUMBER_LENGTH \
            || fraction_part.size() >= MAX_NUMBER_LENGTH \
            || exp_part.size() >= MAX_NUMBER_LENGTH) {

            error("Integer literal too large", start_pos.loc);
        }

        if (is_integer) {
            token = Token::number(
                extract_integer(integral_part, base, start_pos.loc),
                start_pos.loc,
                scanner.view_since(start_pos)
            );
        } else {
            token = Token::number(
                extract_float(integral_part, fraction_part, exp_part, exp_sign, base, start_pos.loc),
                start_pos.loc,
                scanner.view_since(start_pos)
            );
        }

    }

    void parse_name_or_keyword() {
        parse_name();
        assert(token.is_name());

        auto keyword = string_to_keyword.find(token.get_name().value);
        if (keyword != string_to_keyword.end()) {
            token = Token::keyword(keyword->second, token.get_location(), token.get_source());
        }
    }

    void parse_name() {
        auto start_pos = scanner.tell();

        auto value = scanner.read_while<is_name_char>();
        assert(!value.empty());

        token = Token::name(value, start_pos.loc, value);
    }

    /// Returns true if a token has been parsed, false if a comment has been consumed instead
    bool parse_other() {
        auto start_pos = scanner.tell();

        Punct punct{};
        char quote{};

        auto verdict = MiscTrie().feed(scanner, &punct, &quote);

        switch (verdict) {
        case MiscTrie::Verdict::none:
            error(fmt::format("Unexpected character '{}'", (char)scanner.cur()), start_pos.loc);

        case MiscTrie::Verdict::string_quote:
            parse_string(quote);
            return true;

        case MiscTrie::Verdict::block_comment:
            parse_block_comment();
            return false;

        case MiscTrie::Verdict::line_comment:
            parse_line_comment();
            return false;

        case MiscTrie::Verdict::punct:
            token = Token::punct(punct, start_pos.loc, scanner.view_since(start_pos));
            return true;

        NODEFAULT;
        }
    }

    void parse_string(char start_quote) {
        auto start_pos = scanner.tell();
        std::string value = "";

        // Actually, this ended up less efficient than the implementation of this
        // I had in python (due to the fact that I'm using a separate switch-based trie here,
        // instead of walking an explicit one while iterating over the input).
        // The difference should be negligible, though.
        while (true) {
            auto seg_start_pos = scanner.tell();

            char end_quote{};
            int escape{};

            auto verdict = StringTrie().feed(scanner, &end_quote, &escape);

            switch (verdict) {
            case StringTrie::Verdict::none: {
                if (scanner.at_eof()) {
                    error("Unterminated string", start_pos.loc);
                }

                if (scanner.tell() == seg_start_pos) {
                    // So that we don't spin forever in one place
                    scanner.advance();
                }

                value += scanner.view_since(seg_start_pos);
            } break;

            case StringTrie::Verdict::end_quote: {
                if (end_quote == start_quote) {
                    token = Token::string(value, start_quote, start_pos.loc, value);
                    return;
                }

                value += scanner.view_since(seg_start_pos);
            } break;

            case StringTrie::Verdict::escape: {
                if (escape == '\n' || escape == Scanner::end_of_file) {
                    break;
                }

                if (simple_escapes.contains((char)escape)) {
                    value += simple_escapes.at((char)escape);
                    break;
                }

                if (escape == 'x') {
                    int digits[2] = {};
                    digits[0] = scanner.cur();
                    scanner.advance();
                    digits[1] = scanner.cur();
                    scanner.advance();

                    // This automatically checks for invalid digits and EOF
                    unsigned hex_value = evaluate_digit_checked(digits[0], 16, seg_start_pos.loc) * 16 \
                                       + evaluate_digit_checked(digits[1], 16, seg_start_pos.loc);
                    assert(hex_value < 0x100);

                    value += (char)hex_value;
                    break;
                }

                error("Invalid escape sequence", seg_start_pos.loc);
            } break;

            NODEFAULT;
            }
        }
    }

    void parse_line_comment() {
        // TODO: Handle special comments, like pragmas?
        scanner.skip_line();
    }

    void parse_block_comment() {
        // TODO: Allow for different types of block comments?
        auto start_pos = scanner.tell();

        unsigned balance = 1;

        while (balance) {
            auto verdict = BlockCommentTrie().feed(scanner);

            switch (verdict) {
            case BlockCommentTrie::Verdict::none: {
                if (scanner.at_eof()) {
                    error("Unterminated block comment", start_pos.loc);
                }

                scanner.advance();
            } break;

            case BlockCommentTrie::Verdict::start: {
                ++balance;
            } break;

            case BlockCommentTrie::Verdict::end: {
                --balance;
            } break;
            }
        }
    }
    #pragma endregion Tokenization subroutines

    #pragma region Error handling
    [[noreturn]]
    void error(std::string message, const SrcLocation &loc) {
        // TODO: Save or use the loc parameter
        (void)loc;

        throw LexicalError{std::move(message)};
    }
    #pragma endregion Error handling

};

PROMISE_DEFINITION(class, Tokenizer);


#pragma region Out-of-line constants
// TODO: Check if it's okay to have this in the header?
const std::unordered_map<char, char> Tokenizer::simple_escapes = {
    // '\n', '' and 'x' are handled separately
    {'\'', '\''},
    {'\"', '\"'},
    {'\\', '\\'},
    {'a' , '\a'},
    {'b' , '\b'},
    {'f' , '\f'},
    {'n' , '\n'},
    {'r' , '\r'},
    {'t' , '\t'},
    {'v' , '\v'},
};
#pragma endregion Out-of-line constants


}  // namespace bondrewd::lex
