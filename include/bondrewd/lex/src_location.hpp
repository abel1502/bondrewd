#pragma once

#include <bondrewd/internal/common.hpp>

#include <string>
#include <string_view>
#include <fmt/core.h>


namespace bondrewd::lex {


struct SrcLocation {
public:
    #pragma region Fields
    std::string_view filename{""};
    size_t file_pos{0};
    unsigned line{0};
    unsigned column{0};
    #pragma endregion Fields

    #pragma region Constructors
    SrcLocation() = default;

    SrcLocation(std::string_view filename, size_t file_pos, unsigned line, unsigned column) :
        filename{filename}, file_pos{file_pos}, line{line}, column{column} {}
    #pragma endregion Constructors

    #pragma region Service constructors
    SrcLocation(const SrcLocation &) = default;
    SrcLocation(SrcLocation &&) = default;
    SrcLocation &operator=(const SrcLocation &) = default;
    SrcLocation &operator=(SrcLocation &&) = default;
    #pragma endregion Service constructors

    #pragma region to_string
    std::string to_string(bool detailed = false) const {
        if (filename.empty() || !detailed) {
            return fmt::format("{}:{}", line, column);
        }

        return fmt::format("{}:{} in {}", filename, line, column);
    }
    #pragma endregion to_string

    #pragma region Updating
    void advance(int c) {
        ++file_pos;

        switch (c) {
            case '\n':
                ++line;
                column = 0;
                break;

            case '\t':
                column += 4;
                break;

            case '\r':
                break;

            case EOF:
                --file_pos;
                break;

            default:
                ++column;
                break;
        }
    }
    #pragma endregion Updating

    #pragma region Comparisons
    constexpr auto operator<=>(const SrcLocation& other) const {
        return file_pos <=> other.file_pos;
    }

    constexpr bool operator==(const SrcLocation& other) const {
        return file_pos == other.file_pos;
    }

    constexpr bool operator!=(const SrcLocation& other) const = default;
    constexpr bool operator<=(const SrcLocation& other) const = default;
    constexpr bool operator>=(const SrcLocation& other) const = default;
    constexpr bool operator<(const SrcLocation& other) const = default;
    constexpr bool operator>(const SrcLocation& other) const = default;
    #pragma endregion Comparisons

};


}  // namespace bondrewd::lex
