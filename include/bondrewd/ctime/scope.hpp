#pragma once

#include <bondrewd/internal/common.hpp>
#include <bondrewd/ctime/object_ptr.hpp>

#include <unordered_map>
#include <string>
#include <string_view>
#include <concepts>
#include <type_traits>


namespace bondrewd::ctime {


#pragma region Scope
/**
 * This class plays pretty much the same role as dict for Python.
 */
class Scope {
public:
    #pragma region Constructors
    Scope() = default;
    #pragma endregion Constructors

    #pragma region Service constructors
    Scope(const Scope &) = delete;
    Scope(Scope &&) = default;
    Scope &operator=(const Scope &) = delete;
    Scope &operator=(Scope &&) = default;
    #pragma endregion Service constructors

    #pragma region Accessors
    // TODO: Handle missing keys.

    #pragma region get
    object_ptr &get(std::string_view key) {
        return members_str.find(key)->second;
    }

    const object_ptr &get(std::string_view key) const {
        return members_str.find(key)->second;
    }

    // TODO: Handle strings as objects.
    
    object_ptr &get(object_ptr key) {
        return members_obj.at(key);
    }

    const object_ptr &get(object_ptr key) const {
        return members_obj.at(key);
    }
    #pragma endregion get

    #pragma region set
    // TODO
    #pragma endregion set
    #pragma endregion Accessors

protected:
    #pragma region Helpers
    struct _string_hash {
        using is_transparent = std::true_type;

        [[nodiscard]] size_t operator()(const char *txt) const {
            return std::hash<std::string_view>{}(txt);
        }
        
        [[nodiscard]] size_t operator()(std::string_view txt) const {
            return std::hash<std::string_view>{}(txt);
        }

        [[nodiscard]] size_t operator()(const std::string &txt) const {
            return std::hash<std::string>{}(txt);
        }
    };
    #pragma endregion Helpers

    #pragma region Fields
    // Since namespaces allow for both string and object keys,
    // we need to store two maps.
    std::unordered_map<std::string, object_ptr, _string_hash, std::equal_to<>> members_str{};
    std::unordered_map<object_ptr, object_ptr, _string_hash, std::equal_to<>> members_obj{};
    #pragma endregion Fields

};
#pragma endregion Scope


}  // namespace bondrewd::ctime
