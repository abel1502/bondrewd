Language ideas
==============

This document features a list of ideas for key language features. These should
give a general idea of the direction the language will evolve in.

 - **Compile-time metaprogramming**. Ideally, I'm striving for flexibility
    similar to Python's, but entirely at compile-time. As few things as possible
    should be implemented as language features, and instead most things should
    be done through the standard library. If compiler support is required, it
    should be concealed behind a trivial standard library implementation, so
    that from within the language, it should be indistinguishable from something
    implemented in it.
 - **Argument collectors**. In modern languages, function arguments aren't
    limited to a sequence of values. Some support variadic arguments, some
    support keyword ones... I believe this shouldn't be a language feature, but
    rather a library one. For that reason I'm considering an abstraction of an
    argument collector: an object responsible for handling the arguments passed
    to a function. This would also have the added benefit of simplifying the
    process of writing function wrappers, as one could simply reuse the argument
    collector of the wrapped function in the wrapping one. The idea is still
    rather vague, but I'm considering it as one of the crucial features of the
    language.
 - **...**

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

```{code-block} bondrewd
:caption: "`.` vs `::` for attribute access"

class MyInt(@private int32 value);

impl PrivateCtor for MyInt {};

impl MyInt {
    // A static variable
    var total: int32 = 0;

    // Also a static variable, but with a special attribute
    @std::as_type_field
    var total_2: int32 = 0;
};

var my_int_val = MyInt(5);

// To access a static variable of a class, you use `::`
MyInt::total += 1;
my_int_val::total += 1;

// `.` accesses instance attributes. That includes instance attributes of types,
// defined in the corresponding metatype
my_int_val.value += 1;
std::dbg::ctime_assert(MyInt.name == "MyInt");

// However, I think I'd like to have some cool methods on some types, like:
type.of(my_int_val) my_int_val_2;  // Actually, with this specific one there's
    // another problem: `of` must be a ctime function, so it cannot accept
    // non-ctime arguments...
    // One possible solution could be to allow constant-folding functions with
    // non-ctime but unused arguments... Note: we should probably either still
    // compute their expressions at runtime, just to enforce side-effects, or
    // check that no side-effects occur in them. Alternatively, we could allow
    // adding an `unused` marker to an argument, suggesting that its evaluation
    // can be dropped but forbidding its use in the function body. Maybe just
    // having no name could act as such a marker, but that could be confusing.
int32 c = int32::max;
int32 d = int32.parse("123");

// To have static variables accessible through `.`, I'm considering adding a
// special annotation to tell the type factory that it should be that way.
// With it, you have no guarantees agains current or possible future name
// collisions, so it might be a good idea to mark the annotation with a leading
// underscore...
// By the way, static attributes are stored in `MyInt.statics`
MyInt.total_2 += 1;
```

```{code-block} bondrewd
:caption: "Argument collectors"

func increment(
    args: std::args_collector::build().with_arg("a", int32).build()
): int32 => {
    args.a + 1
}

// Any function has a root arg_collector, even if it's defined with normal
// arguments
func logged_increment(args: increment.arg_collector) => {
    log("Before");
    var res = increment(args);
    log("After");
    // TODO: How do I allow to say `return res;` and then not complain about an
    //       implicit unit return? I guess I'd like this block to have a `Never`
    //       for its return expression type, and then have it implicitly cast to
    //       `int32`
    return res;
}
```

```{code-block} bondrewd
:caption: "Example implementation of `type.of(...)`"

trait type {
    // A lot of other things...

    // I'm not sure how template type deduction should be implemented, actually
    func of[T: type](@std::unused_arg value: T) -> type {
        T
    }
};
```
