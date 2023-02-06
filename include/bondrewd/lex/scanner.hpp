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
    #pragma region Constants
    static constexpr size_t BUF_DEFAULT_CAPACITY = 4096;
    static constexpr size_t CHUNK_SIZE = 1024;

    static constexpr int end_of_file = EOF;
    #pragma endregion Constants


    #pragma region Constructors
    Scanner(std::istream &input, std::string_view filename = "") :
        input{input}, loc{filename, 0, 0} {
        
        buf.reserve(BUF_DEFAULT_CAPACITY);
        pull_chunk();

        cached_char = get_at(buf_pos);
    }
    #pragma endregion Constructors

    #pragma region Service constructors
    // TODO: Forbid move?
    Scanner(const Scanner &) = delete;
    Scanner(Scanner &&) = default;
    Scanner &operator=(const Scanner &) = delete;
    Scanner &operator=(Scanner &&) = default;
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

    void advance() {
        loc.advance(cur());
        ++buf_pos;
        cached_char = get_at(buf_pos);
    }

    template <auto P>
    std::string_view read_while() {
        auto start_pos = get_cur_iter();

        while (P(cur())) {
            advance();
        }

        auto end_pos = get_cur_iter();

        return std::string_view(start_pos, end_pos);
    }

    void skip_space() {
        read_while<std::isspace>();
    }

    void skip_line() {
        read_while<[](int c) { return c != '\n' && c != end_of_file; }>();
    }
    #pragma endregion Reading

    #pragma region Location
    const SrcLocation &get_loc() const {
        return loc;
    }
    #pragma endregion Location

protected:
    mutable std::istream &input;
    mutable std::vector<char> buf{};
    size_t buf_pos{0};
    SrcLocation loc;
    int cached_char{end_of_file};

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

    auto get_cur_iter() {
        return buf.begin() + buf_pos;
    }
    #pragma endregion Reading

};


}  // namespace bondrewd::lex