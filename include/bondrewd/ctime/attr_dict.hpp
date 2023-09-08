#pragma once

#include <bondrewd/internal/common.hpp>

#include <unordered_map>


namespace bondrewd::ctime {


#pragma region Forward declarations
class object_ptr;
#pragma endregion Forward declarations


#pragma region AttrDict
// Note: T is always object_ptr, but it's made a template parameter
//       to work around C++'s limitations
template <typename T>
class AttrDict {
public:
    #pragma region Constructor
    AttrDict() = default;
    #pragma endregion Constructor

    #pragma region Service constructors
    AttrDict(const AttrDict &) = delete;
    AttrDict(AttrDict &&) = default;
    AttrDict &operator=(const AttrDict &) = delete;
    AttrDict &operator=(AttrDict &&) = default;
    #pragma endregion Service constructors

    #pragma region API
    T get(std::string_view key) const;

    T get(T key) const;

    void set(std::string_view key, T value);

    void set(T key, T value);

    void remove(std::string_view key);

    void remove(T key);
    #pragma endregion API

protected:
    #pragma region Helper types
    struct attr {
        T value;
        bool is_readonly;
    };
    #pragma endregion Helper types

    #pragma region Fields
    std::unordered_map<T, T> attrs{};
    #pragma endregion Fields

};
#pragma endregion AttrDict


#pragma region Extern templates
extern template class AttrDict<object_ptr>;
#pragma endregion Extern templates


}  // namespace bondrewd::ctime
