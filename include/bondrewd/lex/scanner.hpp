#pragma once

#include <bondrewd/internal/common.hpp>
#include <bondrewd/lex/src_location.hpp>

#include <string>
#include <string_view>
#include <iostream>
#include <vector>


namespace bondrewd::lex {


class Scanner {
public:
    #pragma region Constants and typedefs
    static constexpr size_t BUF_DEFAULT_CAPACITY = 4096;
    static constexpr size_t CHUNK_SIZE = 1024;

    static constexpr int end_of_file = EOF;

    struct state_t {
        size_t buf_pos;
        SrcLocation loc;
    };
    #pragma endregion Constants and typedefs

    #pragma region Constructors
    Scanner(std::istream &input, std::string_view filename = "") :
        input{input}, loc{filename, 0, 0} {
        
        buf.reserve(BUF_DEFAULT_CAPACITY);
        pull_chunk();

        cached_char = get_at(buf_pos);
    }
    #pragma endregion Constructors

    #pragma region Service constructors
    Scanner(const Scanner &) = delete;
    Scanner(Scanner &&) = default;
    Scanner &operator=(const Scanner &) = delete;
    Scanner &operator=(Scanner &&) = delete;
    #pragma endregion Service constructors

    #pragma region Reading
    int cur() const {
        return cached_char;
    }

    int peek_next() const {
        return get_at(buf_pos + 1);
    }

    int peek_prev() const {
        if (buf_pos == 0) {
            return end_of_file;
        }

        return get_at(buf_pos - 1);
    }

    template <auto P>
    std::string_view read_while() {
        auto start_pos = tell();

        while (P(cur())) {
            advance();
        }

        return view_since(start_pos);
    }

    void skip_space() {
        read_while<std::isspace>();
    }

    std::string_view skip_line() {
        auto result = read_while<[](int c) { return c != '\n' && c != end_of_file; }>();
        advance();
        return result;
    }

    std::string_view view_since(const state_t &pos) const {
        assert(pos.buf_pos <= buf.size());

        return std::string_view(buf.begin() + pos.buf_pos, buf.begin() + buf_pos);
    }
    #pragma endregion Reading

    #pragma region Moving
    void advance() {
        loc.advance(cur());
        ++buf_pos;
        cached_char = get_at(buf_pos);
    }

    void advance(size_t count) {
        for (size_t i = 0; i < count; ++i) {
            advance();
        }
    }

    state_t tell() const {
        return {buf_pos, loc};
    }

    void seek(const state_t &pos) {
        assert(pos.buf_pos <= buf.size());

        buf_pos = pos.buf_pos;
        loc = pos.loc;
        cached_char = get_at(buf_pos);
    }
    #pragma endregion Moving

    #pragma region Location
    const SrcLocation &get_loc() const {
        return loc;
    }
    #pragma endregion Location

    #pragma region EOF checking
    bool at_eof() const {
        return cur() == end_of_file;
    }

    operator bool() const {
        return !at_eof();
    }
    #pragma endregion EOF checking

protected:
    #pragma region Fields
    std::istream &input;
    mutable std::vector<char> buf{};
    size_t buf_pos{0};
    SrcLocation loc;
    int cached_char{end_of_file};
    #pragma endregion Fields

    #pragma region Pulling
    void pull_chunk() const {
        if (!input) {
            return;
        }

        buf.resize(buf.size() + CHUNK_SIZE);
        input.read(buf.data() + buf_pos, CHUNK_SIZE);
        buf.resize(buf.size() + input.gcount());
    }

    void pull_all() const {
        while (input) {
            pull_chunk();
        }
    }

    void ensure_next(size_t amount = 1) const {
        ensure_total(buf_pos + amount);
    }

    void ensure_total(size_t amount) const {
        while (amount > buf.size() && input) {
            pull_chunk();
        }
    }
    #pragma endregion Pulling

    #pragma region Reading
    int get_at(size_t pos) const {
        ensure_total(pos + 1);

        if (pos >= buf.size()) {
            return end_of_file;
        }

        return buf[pos];
    }

    auto get_cur_iter() const {
        return buf.begin() + buf_pos;
    }
    #pragma endregion Reading

};


}  // namespace bondrewd::lex
