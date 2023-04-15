#pragma once

#include <bondrewd/internal/common.hpp>
#include <bondrewd/internal/arena.hpp>

#include <concepts>
#include <type_traits>


namespace bondrewd::ctime {


#pragma region Forward Declarations
class Object;
#pragma endregion Forward Declarations


#pragma region Arena
extern util::Arena object_arena;

extern template
class util::arena_ptr<Object, &object_arena>;

using object_ptr = util::arena_ptr<Object, &object_arena>;
#pragma endregion Arena


}  // namespace bondrewd::ctime