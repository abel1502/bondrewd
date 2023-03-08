Language ideas
==============

This document features a list of ideas for key language features. These should
give a general idea of the direction the language will evolve in.

 - [ ] Compile-time metaprogramming. Ideally, I'm striving for flexibility
    similar to Python's, but entirely at compile-time. As few things as possible
    should be implemented as language features, and instead most things should be
    done through the standard library. If compiler support is required, it should
    be concealed behind a trivial standard library implementation, so that from
    within the language, it should be indistinguishable from something
    implemented in it.
 - [ ] ...

## Code samples
This section features some code samples that I'm considering for the language.
I use these to make better decisions about the language design.

```{code-block} bondrewd
:caption: "`class` and `impl` considerations"
cartridge test;


class Foo[T: type] {
    bar: int32,
    baz: &uint64,
    field: T
};


// Bad, I guess, because that would imply sequential evaluation of methods
// ... unless I make it sequential for references within the impl scope,
// but allow to use everything in method scopes. Like Python does.
impl[T: type] Foo[T] {
    if (T != int32) {
        public func sum(&self): uint64 => {
            bar + *baz
        };
    };
};
```

```{code-block} bondrewd
:caption: "Function definition syntax options"
// 1. Optional trailing return type in the form of `-> type`;
//    required `=>` before the body; any expression for the body.
// func foo(a: int32, b: int32) -> int32 => {
//     a + b
// };

// I guess I prefer this one
// 2. Same as 1, but with `: type` for the return type instead.
func foo(a: int32, b: int32): int32 => {
    a + b
};
```

```{code-block} bondrewd
:caption: "Class definition in a function-like manner"
// Essentially, you specify the constructor signature instead of the fields.
// This means that static fields must be declared in an `impl` block, along
// with methods. I'm considering allowing one `impl` block to be implicitly
// declared as a braced block immediately after the class definition...
class Foo[T: type] (
    bar: int32,
    baz: &uint64,
    field: T
);
```

```{code-block} bondrewd
:caption: "`ctime` declarations"

// `ctime` functions could be declared in one of two ways:
// (I should pick one of these)

// 1. With a `ctime` keyword before the function body
//    (Essentially, just by making its result a compile-time expression)
func foo(a: int32, b: int32): int32 => ctime {
    a + b
};

// 2. With a `ctime` keyword before the declaration
ctime func foo(a: int32, b: int32): int32 => {
    a + b
};

// `ctime` classes are unambiguous:
ctime class Foo(int32 a, int32 b);

// Same thing goes for `ctime` traits:
ctime trait Bar {
    func sum(&self): int32;
};

// `ctime` impls are (probably) the same thing:
// (A `ctime` impl is only applicable to `ctime` classes and `ctime` traits)
ctime impl Bar for Foo {
    // All vars and methods are implicitly `ctime`
    func sum(&self): int32 => self.a + self.b;
};
```

