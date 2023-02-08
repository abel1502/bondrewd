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
    Lexer(const Lexer &) = default;
    Lexer(Lexer &&) = default;
    Lexer &operator=(const Lexer &) = default;
    Lexer &operator=(Lexer &&) = default;
    #pragma endregion Service constructors

    #pragma region Token access
    const Token &cur() const {
        return get_at(index);
    }

    const Token &peek(int offset) const {
        return get_at(index + offset);
    }
    #pragma endregion Token access

    #pragma region End checking
    bool at_end() const {
        return !tokens.empty() && tokens.back().is_endmarker();
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
    std::ostream &dump(std::ostream &stream) const {
        stream << "Lexer([";

        const size_t size = tokens.size();
        for (size_t i = 0; i < size; ++i) {
            tokens[i].dump(stream);
            
            if (i == index) {
                stream << " *";
            }

            if (i < size - 1) {
                stream << " ";
            }
        }

        stream << "])";

        return stream;
    }
    #pragma endregion Debug

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
    const Token &get_at(size_t pos) const {
        ensure_total(pos + 1);

        if (pos >= tokens.size()) {
            return tokens.back();
        }

        return tokens[pos];
    }
    #pragma endregion Reading

};


}  // namespace bondrewd::lex
