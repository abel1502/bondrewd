#pragma once

#include <bondrewd/internal/common.hpp>
#include <bondrewd/ctime/object.hpp>

#include <concepts>
#include <type_traits>


namespace bondrewd::ctime {


#pragma region TraitMeta
/// The built-in implementation of a trait.
/// Satisfies the `Trait` trait.
class TraitMeta : public Object {
public:
    #pragma region Constructors
    // TODO
    #pragma endregion Constructors

    #pragma region API
    // TODO: Subject to change!

    virtual bool is_implemented_for(object_ptr obj) const {
        return get_implementation(std::move(obj)) != nullptr;
    }

    virtual object_ptr get_implementation(object_ptr obj) const;

    virtual void implement_for(object_ptr obj, object_ptr impl);
    #pragma endregion API

protected:
    //

};
#pragma endregion TraitMeta


#pragma region TraitBody
/// The built-in implementation of an implemented trait for a specific type.
/// Doesn't have a corresponding trait, as of now.

#pragma region trait_slot
// TODO
#pragma endregion trait_slot

// TODO: Probably implement in a different file
#pragma endregion TraitBody


#pragma region TraitTrait
/// The `Trait` trait
// TODO
#pragma endregion TraitTrait


}  // namespace bondrewd::ctime
