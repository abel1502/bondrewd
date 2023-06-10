Data model
==========

The three fundamental concepts behind Bondrewd's data model are _object_,
_type_ and _trait_. Everything in Bondrewd is an object (including types and
traits). Every object has a type. Types may have traits implemented for them.

A type may be considered as the minimal structural unit with which the static
type checking may operate. For runtime objects, in compiled code, polymorphism
is inherently limited to values within a single type.

A trait may be considered as a category of types, as well as some functionality
associated with them. A trait has a set of _slots_ (which respresent the
associated functionality).
% TODO: Elaborate!

An object is said to satisfy a trait if the trait is implemented for its type.
An object is a type if it satisfies the `Type` trait. An object is a trait if
it satisfies the `Trait` trait.

```{note}
If you want to create a custom type or a trait, pay attention that the `Type`
or `Trait` traits should NOT be implemented for the actual object you're
creating, but for its type. To avoid confusion, the term _metatype_ is used to
refer to the type of a type or a trait.
```

## Core traits
`Trait`
: This trait signifies that an object is a trait.

`Type`
: This trait signifies that an object is a type.

`Any`
: This trait is implemented for all types. It has no slots. It's used to
  represent the most general type constraint.

% TODO: More!

_Unfinished..._