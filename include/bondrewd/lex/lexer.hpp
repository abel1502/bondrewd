#pragma once

#include <bondrewd/internal/common.hpp>
#include <bondrewd/lex/tokenizer.hpp>

#include <vector>
#include <iostream>


namespace bondrewd::lex {


/**
 * A caching wrapper over a Tokenizer.
 */
class Lexer {
public:
    #pragma region Constants and typedefs
    using state_t = size_t;
    #pragma endregion Constants and typedefs

    #pragma region Constructors
    Lexer(Tokenizer tokenizer) :
        tokenizer{std::move(tokenizer)} {}
    #pragma endregion Constructors

    #pragma region Service constructors
    Lexer(const Lexer &) = delete;
    Lexer(Lexer &&) = default;
    Lexer &operator=(const Lexer &) = delete;
    Lexer &operator=(Lexer &&) = default;
    #pragma endregion Service constructors

    #pragma region Factories
    static Lexer from_stream(std::istream &input, std::string_view filename = "") {
        return Lexer{Tokenizer::from_stream(input, filename)};
    }

    static Lexer from_string(std::string_view input, std::string_view filename = "") {
        return Lexer{Tokenizer::from_string(input, filename)};
    }

    static Lexer from_file(std::filesystem::path filename) {
        return Lexer{Tokenizer::from_file(filename)};
    }
    #pragma endregion Factories

    #pragma region Token access
    Token &cur() const {
        return get_at(index);
    }

    Token &peek(int offset) const {
        return get_at(index + offset);
    }
    #pragma endregion Token access

    #pragma region End checking
    bool at_end() const {
        return !tokens.empty() && tokens.back().is_endmarker();
    }

    operator bool() const {
        return !at_end();
    }
    #pragma endregion End checking

    #pragma region Positioning
    void advance() {
        ++index;
    }

    void advance(size_t count) {
        index += count;
    }

    state_t tell() const {
        return index;
    }

    void seek(state_t pos) {
        assert(pos <= tokens.size());

        index = pos;
    }
    #pragma endregion Positioning

    #pragma region Debug
    std::ostream &dump(std::ostream &stream = std::cout) const {
        stream << "Lexer([";

        const size_t size = tokens.size();
        for (size_t i = 0; i < size; ++i) {
            if (i > 0) {
                stream << " ";
            }

            tokens[i].dump(stream);
            
            if (i == index) {
                stream << " *";
            }
        }

        if (!at_end()) {
            stream << " ...";
        }

        stream << "])";

        return stream;
    }
    #pragma endregion Debug

    #pragma region Expecting
    #pragma region Proxies
    struct _ExpectProxy {
    public:
        _ExpectProxy(Lexer *lexer_) :
            lexer{lexer_} {}
        
        _ExpectProxy(const _ExpectProxy &) = default;
        _ExpectProxy(_ExpectProxy &&) = default;
        _ExpectProxy &operator=(const _ExpectProxy &) = default;
        _ExpectProxy &operator=(_ExpectProxy &&) = default;

        Token *token(TokenType type) {
            lexer->ensure_next();

            Token *result = &lexer->cur();
            if (result->get_type() != type) {
                return nullptr;
            }

            lexer->advance();

            return result;
        }

        Token *keyword(HardKeyword keyword) {
            Token *result = token(TokenType::keyword);

            if (result == nullptr || result->get_keyword().value != keyword) {
                return nullptr;
            }

            return result;
        }

        Token *punct(Punct punct) {
            Token *result = token(TokenType::punct);

            if (result == nullptr || result->get_punct().value != punct) {
                return nullptr;
            }

            return result;
        }

        Token *soft_keyword(const std::string_view &keyword) {
            Token *result = token(TokenType::name);

            if (result == nullptr || result->get_name().value != keyword) {
                return nullptr;
            }

            return result;
        }

    protected:
        Lexer *lexer;

    };

    struct _LookaheadProxy : private _ExpectProxy {
    public:
        _LookaheadProxy(Lexer *lexer_, bool positive) :
            _ExpectProxy(lexer_),
            state{lexer->tell()},
            positive{positive} {}
        
        _LookaheadProxy(const _LookaheadProxy &) = default;
        _LookaheadProxy(_LookaheadProxy &&) = default;
        _LookaheadProxy &operator=(const _LookaheadProxy &) = default;
        _LookaheadProxy &operator=(_LookaheadProxy &&) = default;

        ~_LookaheadProxy() {
            lexer->seek(state);
        }

        #define WRAP_(NAME, ARG_TYPE) \
            bool NAME(ARG_TYPE arg) { \
                return ((bool)_ExpectProxy::NAME(arg)) == positive; \
            }
        
        WRAP_(token, TokenType);
        WRAP_(keyword, HardKeyword);
        WRAP_(punct, Punct);
        WRAP_(soft_keyword, std::string_view);

        #undef WRAP_
    
    protected:
        Lexer::state_t state;
        bool positive;

    };
    #pragma endregion Proxies
    
    // TODO: Wrappers for these on the parser level.
    //       Strictly speaking, these are only the atomic versions
    //       of the lookahead/expect methods.
    
    /**
     * Usage examples:
     *  - `.expect().token(TokenType::keyword)`
     *  - `.expect().keyword(HardKeyword::if)`
     *  - `.expect().punct(Punct::semicolon)`
     *  - `.expect().soft_keyword("aboba")`
     */
    auto expect() {
        return _ExpectProxy{this};
    }

    /**
     * Usage examples:
     *  - `.lookahead(true).token(TokenType::keyword)`
     *  - `.lookahead(false).keyword(HardKeyword::if)`
     *  - `.lookahead(true).punct(Punct::semicolon)`
     *  - `.lookahead(false).soft_keyword("aboba")`
     * 
     * Can also be used as a reset-guard:
     * ```
     * {
     *     auto guard = lexer.lookahead(true);  // or false, doesn't matter
     * 
     *     ...
     * }
     * ```
     */
    auto lookahead(bool positive) {
        return _LookaheadProxy{this, positive};
    }
    #pragma endregion Expecting

protected:
    #pragma region Fields
    mutable Tokenizer tokenizer;
    mutable std::vector<Token> tokens{};
    size_t index{0};
    #pragma endregion Fields

    #pragma region Pulling
    // While this does resemble Scanner a lot, it actually operates by a bit different principle
    void pull_one() const {
        if (at_end()) {
            return;
        }

        tokens.push_back(tokenizer.get_token());
    }

    void ensure_next(size_t amount = 1) const {
        ensure_total(index + amount);
    }

    void ensure_total(size_t amount) const {
        while (amount > tokens.size() && !at_end()) {
            pull_one();
        }
    }
    #pragma endregion Pulling
    
    #pragma region Reading
    Token &get_at(size_t pos) const {
        ensure_total(pos + 1);

        if (pos >= tokens.size()) {
            return tokens.back();
        }

        return tokens[pos];
    }
    #pragma endregion Reading

};


}  // namespace bondrewd::lex
