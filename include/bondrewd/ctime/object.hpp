#pragma once

#include <bondrewd/internal/common.hpp>
#include <bondrewd/ctime/object_ptr.hpp>
#include <bondrewd/ctime/scope.hpp>

#include <concepts>
#include <type_traits>


namespace bondrewd::ctime {


#pragma region Object
class Object {
public:
    //

protected:
    #pragma region Fields
    object_ptr type;
    #pragma endregion Fields

};
#pragma endregion Object


}  // namespace bondrewd::ctime