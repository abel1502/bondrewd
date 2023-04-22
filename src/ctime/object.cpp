#include <bondrewd/ctime/object.hpp>


namespace bondrewd::ctime {


object_ptr Object::get_provided_trait_impl(object_ptr trait) const {
    if (object_ptr cached = traits.get_trait(trait)) {
        return cached;
    }

    // TODO: Look for a templated impl and cache it, if found.

    return nullptr;
}


}  // namespace bondrewd::ctime
