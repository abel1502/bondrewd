#pragma once

#include <bondrewd/internal/common.hpp>
#include <bondrewd/lex/scanner.hpp>
#include <bondrewd/lex/token.hpp>

#include <cctype>
#include <exception>
#include <fmt/core.h>


namespace bondrewd::lex {


// TODO: Add location info!
// (Either to the error or to the lexer state)
DECLARE_ERROR(LexerError, std::runtime_error);


JUST_DECLARE
class Lexer {
public:
    #pragma region Constructors
    Lexer(Scanner source) :
        scanner{std::move(source)} {}
    #pragma endregion Constructors

    #pragma region Service constructors
    Lexer(const Lexer &) = delete;
    Lexer(Lexer &&) = delete;
    Lexer &operator=(const Lexer &) = delete;
    Lexer &operator=(Lexer &&) = delete;
    #pragma endregion Service constructors

    #pragma region Interface
    /**
     * Get the next token from the input stream.
     * 
     * Note: caching is expected to be done by the caller.
     * 
     * @throws LexerError if the next token is invalid.
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

        token = Token::endmarker(scanner.loc(), "");
        return token;
    }
    #pragma endregion Interface

protected:
    #pragma region Fields
    Scanner scanner;
    Token token{Token::endmarker(SrcLocation{}, "")};
    #pragma endregion Fields

    #pragma region Private constants
    static const std::unordered_map<char, char> simple_escapes = {
        // '\n', '' and 'x' are handled separately
        '\'': '\'',
        '\"': '\"',
        '\\': '\\',
        'a': '\a',
        'b': '\b',
        'f': '\f',
        'n': '\n',
        'r': '\r',
        't': '\t',
        'v': '\v',
    };
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

    static unsigned evaluate_digit(int c) {
        if ('0' <= c && c <= '9') {
            return c - '0';
        }
        if ('a' <= c && c <= 'z') {
            return c - 'a' + 10;
        }
        if ('A' <= c && c <= 'Z') {
            return c - 'A' + 10;
        }

        throw LexerError{fmt::format("Invalid digit '{}'", (char)c)};
    }

    static unsigned evaluate_digit(int c, unsigned base) {
        assert(2 <= base && base <= 36);

        unsigned result = evaluate_digit(c);

        if (result >= base) {
            throw LexerError{fmt::format("Wrong digit '{}' for base {}", (char)c, base)};
        }

        return result;
    }
    #pragma endregion Character classification

    #pragma region Tokenization subroutines
    void parse_number() {
        throw LexerError{"Numbers are not yet supported"};
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

        auto value = scanner.read_while<is_name_char>(token.get_name().value);
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
            throw LexerError{"Invalid character"};

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
                    throw LexerError{"Unterminated string"};
                }

                if (scanner.tell() == seg_start_pos) {
                    // So that we don't spin forever in one place
                    scanner.advance();
                }

                value += scanner.view_since(seg_start_pos);
            } break;
            
            case StringTrie::Verdict::end_quote: {
                if (end_quote == start_quote) {
                    token = Token::string(value, start_pos.loc, value);
                    return;
                }

                value += scanner.view_since(seg_start_pos);
            } break;
            
            case StringTrie::Verdict::escape: {
                if (simple_escapes.contains(escape)) {
                    value += simple_escapes.at(escape);
                    break;
                }

                if (escape == '\n' || escape == Scanner.end_of_file) {
                    break;
                }

                if (escape == 'x') {
                    int digits[2] = {};
                    digits[0] = scanner.cur();
                    scanner.advance();
                    digits[1] = scanner.cur();
                    scanner.advance();
                    
                    // This automatically checks for invalid digits and EOF
                    unsigned value = evaluate_digit(digits[0], 16) * 16 + evaluate_digit(digits[1], 16);
                    assert(value < 0x100);

                    value += (char)value;
                    break;
                }
            } break;

            NODEFAULT;
            }
        }
    }
    
    void parse_line_comment() {
        // TODO: Handle special comments?
        scanner.skip_line();
    }

    void parse_block_comment() {
        // TODO: Allow for different types of block comments?

        unsigned balance = 1;

        while (balance) {
            auto verdict = BlockCommentTrie().feed(scanner);

            switch (verdict) {
            case BlockCommentTrie::Verdict::none: {
                if (scanner.at_eof()) {
                    throw LexerError{"Unterminated block comment"};
                }

                scanner.advance();
            } break;

            case BlockCommentTrie::Verdict::open: {
                ++balance;
            } break;

            case BlockCommentTrie::Verdict::close: {
                --balance;
            } break;
        }
    }
    #pragma endregion Tokenization subroutines

};

PROMISE_DEFINITION(class Lexer);


}  // namespace bondrewd::lex
