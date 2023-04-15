#include <bondrewd/ctime/object.hpp>



namespace bondrewd::ctime {


#pragma region Arena
util::Arena object_arena{};

template
class util::arena_ptr<Object, &object_arena>;
#pragma endregion Arena


}  // namespace bondrewd::ctime
