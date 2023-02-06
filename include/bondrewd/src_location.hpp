#pragma once

#include <bondrewd/_common.hpp>

#include <string>
#include <string_view>
#include <fmt/core.h>


namespace bondrewd::lex {


struct SrcLocation {
public:
    #pragma region Fields
    std::string_view filename{""};
    unsigned line{0};
    unsigned column{0};
    #pragma endregion Fields

    #pragma region Constructors
    SrcLocation() = default;

    SrcLocation(std::string_view filename, unsigned line, unsigned column) :
        filename{filename}, line{line}, column{column} {}
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

};


}  // namespace bondrewd::lex
