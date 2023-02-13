#pragma once

#include <bondrewd/internal/common.hpp>
#include <bondrewd/lex/src_location.hpp>

#include <string>
#include <string_view>
#include <iterator>
#include <concepts>
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cctype>


namespace bondrewd::lex {


class Scanner {
public:
    #pragma region Constants and typedefs
    static constexpr int end_of_file = EOF;

    struct state_t {
        size_t buf_pos;
        SrcLocation loc;

        constexpr auto operator<=>(const state_t& other) const {
            return buf_pos <=> other.buf_pos;
        }

        constexpr bool operator==(const state_t& other) const {
            return buf_pos == other.buf_pos;
        }

        constexpr bool operator!=(const state_t& other) const = default;
        constexpr bool operator<=(const state_t& other) const = default;
        constexpr bool operator>=(const state_t& other) const = default;
        constexpr bool operator<(const state_t& other) const = default;
        constexpr bool operator>(const state_t& other) const = default;
    };
    #pragma endregion Constants and typedefs

    #pragma region Constructors
    template <std::input_iterator I, std::sentinel_for<I> S>
    Scanner(I src, S end, std::string_view filename = "") :
        buf{std::move(src), std::move(end)}, loc{filename, 0, 0} {

        cached_char = get_at(buf_pos);
    }
    #pragma endregion Constructors

    #pragma region Service constructors
    Scanner(const Scanner &) = delete;
    Scanner(Scanner &&) = default;
    Scanner &operator=(const Scanner &) = delete;
    Scanner &operator=(Scanner &&) = default;
    #pragma endregion Service constructors

    #pragma region Factories
    static Scanner from_stream(std::istream &input, std::string_view filename = "") {
        using iter = std::istream_iterator<char>;

        return Scanner{iter{input}, iter{}, filename};
    }

    static Scanner from_string(std::string_view input, std::string_view filename = "") {
        return Scanner{input.begin(), input.end(), filename};
    }

    static Scanner from_file(std::filesystem::path filename) {
        std::ifstream stream{filename};

        return from_stream(stream, filename.string());
    }
    #pragma endregion Factories

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

    template <typename T>
    std::string_view read_while(T predicate) {
        auto start_pos = tell();

        while (predicate(cur())) {
            advance();
        }

        return view_since(start_pos);
    }

    void skip_space() {
        read_while<isspace>();
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

    #pragma region Positioning
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
    #pragma endregion Positioning

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
    std::vector<char> buf{};
    size_t buf_pos{0};
    SrcLocation loc;
    int cached_char{end_of_file};
    #pragma endregion Fields

    #pragma region Reading
    int get_at(size_t pos) const {
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
