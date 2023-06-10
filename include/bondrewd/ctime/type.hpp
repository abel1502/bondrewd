#pragma once

#include <bondrewd/internal/common.hpp>
#include <bondrewd/ctime/object.hpp>

#include <concepts>
#include <type_traits>


namespace bondrewd::ctime {


#pragma region TraitSet
/// This is a container that stores the traits implemented for an object.
/// The keys in the maps are trait objects, and the values are the impl objects.
class TraitSet {
public:
    #pragma region Constructors
    TraitSet() = default;
    #pragma endregion Constructors

    #pragma region Service constructors
    TraitSet(const TraitSet &) = delete;
    TraitSet(TraitSet &&) = default;
    TraitSet &operator=(const TraitSet &) = delete;
    TraitSet &operator=(TraitSet &&) = default;
    #pragma endregion Service constructors

    #pragma region API
    /// Note: nullptrs should only be cached if an explicit negative impl is given.
    void add_trait(object_ptr trait, object_ptr impl);

    /// Note: see add_traits.
    /// This method is const, because weak_traits is a mutable cache
    void add_weak_trait(object_ptr trait, object_ptr impl) const;

    /// Note: Doesn't check for non-cached templated impls.
    /// Returns nullptr if the trait is not implemented.
    object_ptr get_trait(object_ptr trait) const;
    #pragma endregion API

protected:
    #pragma region Fields
    // TODO: Use weak pointers as a small optimization, perhaps?
    std::hash_map<object_ptr, object_ptr> traits{};
    mutable std::hash_map<object_ptr, object_ptr> weak_traits{};
    #pragma endregion Fields

};
#pragma endregion TraitSet


#pragma region TypeMeta
/// The built-in implementation of a type.
/// Satisfies the `Type` trait.
class TypeMeta : public Object {
public:
    #pragma region API
    /// Returns the trait implementation for the given trait.
    /// If a template implementation is found, it is  also cached as a weak trait.
    /// Note that the queried traits do not affect this object,
    /// but those for which it is the type. If you're interested
    /// in the traits that would affect this object, use
    /// `get_object_trait_impl` instead.
    object_ptr get_trait_impl(object_ptr trait) const;
    #pragma endregion API

protected:
    #pragma region Fields
    TraitSet traits{};
    #pragma endregion Fields
};
#pragma endregion TypeMeta


#pragma region TypeTrait
/// The `Type` trait.
// TODO: Implement
#pragma endregion TypeTrait


// TODO: impl Trait for Type
//       impl Type for TypeMeta


}  // namespace bondrewd::ctime
