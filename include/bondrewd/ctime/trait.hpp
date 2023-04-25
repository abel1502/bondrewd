#pragma once

#include <bondrewd/internal/common.hpp>
#include <bondrewd/ctime/object.hpp>

#include <concepts>
#include <type_traits>


namespace bondrewd::ctime {


class Trait : public Object {
public:
    #pragma region Constructors
    // TODO
    #pragma endregion Constructors

    #pragma region Service constructors
    Trait(const Trait &other) = delete;
    Trait(Trait &&other) = delete;
    Trait &operator=(const Trait &other) = delete;
    Trait &operator=(Trait &&other) = delete;
    #pragma endregion Service constructors

protected:
    //

};


}  // namespace bondrewd::ctime
