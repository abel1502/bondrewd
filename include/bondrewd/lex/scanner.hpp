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
    #pragma endregion Constants and typedefs

    #pragma region Constructors
    template <std::input_iterator I, std::sentinel_for<I> S>
    Scanner(I src, S end, std::string_view filename = "") :
        buf{std::move(src), std::move(end)}, loc{filename, 0, 0, 0} {

        cached_char = get_at(loc.file_pos);
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
        return get_at(loc.file_pos + 1);
    }

    int peek_prev() const {
        if (loc.file_pos == 0) {
            return end_of_file;
        }

        return get_at(loc.file_pos - 1);
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

    std::string_view view_since(const SrcLocation &pos) const {
        assert(pos.file_pos <= buf.size());
        assert(pos.file_pos <= loc.file_pos);

        return std::string_view(buf.begin() + pos.file_pos, buf.begin() + loc.file_pos);
    }

    std::string_view view_line(const SrcLocation &pos) const {
        assert(pos.file_pos <= buf.size());

        constexpr auto is_newline = [](char c) { return c == '\n'; };

        auto start = std::find_if(buf.rend() - pos.file_pos, buf.rend(), is_newline);
        auto end = std::find_if(buf.begin() + pos.file_pos, buf.end(), is_newline);

        return std::string_view(start.base(), end);
    }

    std::string_view view_context(const SrcLocation &pos, size_t context_size = 5) const {
        assert(pos.file_pos <= buf.size());

        constexpr auto is_newline = [](char c) { return c == '\n'; };

        auto point = buf.begin() + pos.file_pos;

        auto start = std::max(point - context_size, buf.begin());
        auto end = std::min(point + context_size, buf.end());

        if (auto start_nl = std::find_if(start, point, is_newline); start_nl != point) {
            start = start_nl + 1;
        }

        if (auto end_nl = std::find_if(point, end, is_newline); end_nl != end) {
            end = end_nl;
        }

        return std::string_view(start, end);
    }
    #pragma endregion Reading

    #pragma region Positioning
    void advance() {
        loc.advance(cur());
        cached_char = get_at(loc.file_pos);
    }

    void advance(size_t count) {
        for (size_t i = 0; i < count; ++i) {
            advance();
        }
    }

    SrcLocation tell() const {
        return loc;
    }

    void seek(const SrcLocation &pos) {
        assert(pos.file_pos <= buf.size());

        loc = pos;
        cached_char = get_at(loc.file_pos);
    }
    #pragma endregion Positioning

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
        return buf.begin() + loc.file_pos;
    }
    #pragma endregion Reading

};


}  // namespace bondrewd::lex
