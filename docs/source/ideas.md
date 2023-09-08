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
 - **~~Argument collectors~~**. In modern languages, function arguments aren't
    limited to a sequence of values. Some support variadic arguments, some
    support keyword ones... I believe this shouldn't be a language feature, but
    rather a library one. For that reason I'm considering an abstraction of an
    argument collector: an object responsible for handling the arguments passed
    to a function. This would also have the added benefit of simplifying the
    process of writing function wrappers, as one could simply reuse the argument
    collector of the wrapped function in the wrapping one. The idea is still
    rather vague, but I'm considering it as one of the crucial features of the
    language.

    ... or not. The more I think about it, the more it seems to just
    overcomplicate things. I guess it would be better to just introduce
    keyword and variadic arguments as language features...
    But I really do like the idea of saying "take the same arguments as this
    function"...

    Maybe instead of collectors I should introduce "argument acceptors":
    objects responsible for turning an AST expression into some sort of value.
    Can `unused` be implemented through this?
 - **Macros**. I'm pretty certain I want to be able to influence ASTs with
    (procedural) macros. Token stream-based macros seem cool, but might cause
    issues with namespace encapsulation and stuff like that. Maybe it would be
    fine if macros had a way to specify a limit of what grammatical constructs
    they could generate. That way the compiler can be sure that the macro won't
    create new scopes, for instance. Alternatively, we could just limit all
    token-based macros to creating exclusively expressions. The only things it
    would prevent them from creating are `impl`s, namespaces, variable
    declarations (in the immediate scope) and assignments. Maybe we could even
    allow statements as well, as long as the result is balanced (i.e. can be
    parsed completely). Also not sure how I feel about macros defining new
    macros. Overall, still got things to think about here.
 - **Generalized namespaces**. Various attribute access is ubiquitous in modern
    programming languages. But in all cases known to me, it boils down to a
    string-keyed mapping. Coupled with some thoughts on traits, this gave me an
    idea to generalize namespaces: a key can be any (`ctime`) object. I'm
    considering two possible syntaxes for this: either `a::(74)` or 
    `a::for 74`. (Note that this isn't about `.` vs `::` --- I intend to have
    both with slightly different meanings). The first one probably would be more
    clear (for example, what would `a::for b::c` mean? Introducing new
    unapparent precedence rules goes against my design intentions). Normal
    attribute access would then be equivalent to `a::("attr")` or
    `a::for "attr"`. Come to think of it, a hybrid syntax might be a good idea:
    `a::(for 74)`.

    It is important to note that both objects play a part in generalized
    attribute resolution (which should be reflected in the attribute access
    override mechanism). Probably the left-hand side object would be queried
    first, and if it doesn't provide the attribute, the right-hand side object
    would be queried (in a distinct way, though --- I don't want attribute
    access to be symmetric by any means). This way, for example, the
    'trait-for-type' namespace could be expressed as `Foo::(for Barable)`. I'm
    hoping with this and a few other changes, I could even make traits a library
    feature...
 - **Generalized types**. It's desirable to be able to specify, for instance, a
    trait as the constraint for an argument of a function, automatically
    creating a template. However, at this point, it would require special
    support from the compiler. An alternative I'm considering is to call structs
    and builtin types something like "specific types", and to allow type
    annotations to be "generalized types". A generalized type should be able to
    tell is another type matches it or not (Still undecided about whether it
    should only concern specific types, or also generalized ones). This is a
    very raw idea though, I still have a lot to think about here.

    I guess a "specific type" is something that can be used to compile value
    manipulations. (Maybe this implies that there should be a separate
    metaclass for `ctime`-types). It could actually have separate methods for
    compile-time and run-time manipulations, allowing to make all `ctime` values
    to be passed by-reference regardless of attributes...
 - **Mutable types**. This is still a very vague idea, but I'd like to have a
    way to change a variable's type 'dynamically' at compile time. For example,
    it would be nice to have the File type automatically track that it's been
    opened before it's used, and closed before it's destroyed. This can already
    be implemented, but comes at a run-time cost. My idea for a solution is to
    allow the object to store some information about itself at compile time.
    I'm not yet sure about the details, but I'm considering either something
    like mutable template parameters, or, alternatively, some sort of `ctime`
    fields (for non-`ctime` classes). One thing to note is that this cannot
    always be properly computed at compile time, so providing the only available
    File API in this way would be detremental. The better approach would be a
    wrapper around normal File that would implement the checks. If the further
    use cases I come up with could also be decomposed this way, I'll consider
    enforcing this specific mechanism in some way.

    Actually, this has lead me to consider the compile-time representation of
    objects. From what I've come up with so far, the object, from the
    perspective of the compiler, seems to consist of the type (a reference to a
    `ctime`-available object), an optional value (represented how?), which is
    only present for successfully constant-folded expressions, and a reference
    to some `ctime` object responsible for implementing manipulations with the
    object (perhaps only present for runtime objects, but maybe not). This is
    actually a little depressing in a way --- to represent an object, we need
    1 to 3 other objects... But something like this has already been done in
    other languages (like Python), so I guess it's not that bad. Maybe the value
    could be an AST tree...? No, I don't think so, actually. It's already done
    the other way --- objects can be embedded into the AST in the form of
    `Constant` nodes.
 - **Asynchronous compilation**. This isn't, strictly speaking, a language
    feature, in that it isn't exposed to the user. But it's a crucial concept
    behind forward reference resolution in complex contexts. Essentially, the
    idea is to make compilation of every function asynchronous, and whenever a
    ctime operation cannot be resolved, to suspend the compilation until the
    corresponding object is available. Note that, to support recursion between
    functions and other similar constructs, objects would have to be defined in
    parts, as soon as they become available. So, for a function, the ctime
    status and the signature would appear before the body (unless the return
    type is to be deduced from the body, this case I'm not sure about how
    to handle...). This system probably won't eliminate the need for
    incomplete types (_Generalized types_), though...
 - **Cartridges**. I'm still very much undecided on how I'd like to implement
    the module ('cartridge') system. The current stub way of specifying the
    cartridge in the first line of the file really doesn't appeal to me. But,
    crucially, I don't want to tie cartridges to the filesystem. I really like
    how C++ namespaces separate the logical and physical organization of code.
 - **~~Implicit `ctime`, explicit `runtime`~~**. I'm beginning to think that
    `ctime` should be the default, and runtime code should be explicitly marked
    as such. This is due to the fact that, starting at the root of the file, it
    actuallyccontains `ctime` code. All sorts of definitions (classes, traits,
    functions, namespaces, impls, vars and so on) are actually compile-time
    code, which may incur something for run-time (like a reserved space or
    function bytecode in memory), but nothing that would be compiled into any
    sort of assembly code. The first (and only) place where runtime code could
    appear is inside a runtime function, so it makes a lot more sense to mark
    those, as opposed to everything else. Maybe we could even omit the explicit
    marking, and instead assume that a function is runtime only if it contains a
    runtime-exclusive operation (i.e. calling another runtime function, or
    accessing a runtime variable). One potential problem is that it might be
    counter-intuitive to some users, but I guess I'm willing to make that
    sacrifice for the sake of principle.
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
:caption: "~~Argument collectors~~"

func increment(a: int32): int32 => {
    a + 1
};

func logged_increment(args: increment.args_def) => {
    log("Before");
    var res = increment(args);
    log("After");
    // TODO: How do I allow to say `return res;` and then not complain about an
    //       implicit unit return? I guess I'd like this block to have a `Never`
    //       for its return expression type, and then have it implicitly cast to
    //       `int32`
    return res;
};

func foo(
    a: int32,
    b: int32 = 5,
    @std::varargs c: int32[],
    @std::unused_arg d: int32,
): int32 => {
    a + b + c.sum()  /* + d  /* nope - can't use an unused arg's value */ */
};
```

```{code-block} bondrewd
:caption: "Example implementation of `type.of(...)`"

trait type {
    // A lot of other things...

    // I'm not sure how template type deduction should be implemented, actually
    func of[T: type](@std::unused_arg value: T) -> type {
        T
    };
    
    // No, you know what, this is bad. I'd much prefer to provide type.of as a
    // compiler builtin, and then implement type deduction in the library with
    // it.
};
```

```{code-block} bondrewd
:caption: "Imposing trait requirements on a type value"

trait Foo {
    func foo(&self) -> Unit;
};

// These two definitions are essentially the same thing, with two exceptions:
//  - The second case checks that `T` implements `Foo` at compile-time
//    (which you can also do manually in the first case, though)
//  - The second case doesn't allow to explicitly specify the type of `value`
//    (although I'm considering adding a mechanism to get the result of
//     overload resolution as an object...)

func bar[T: type](value: T) => {
    value.foo();
};

func baz(value: Foo) => {
    value.foo();
};
```

```{code-block} bondrewd
:caption: "Forward references"

// In functions: will work, because function body is interpreted lazily
func f1() => {
    f2();
};

func f2() => {};

// In classes: will not work as is...
// Maybe we should interpret the class fields lazily as well...
class C1(
    // By the way, this syntax for references might turn out problematic, since
    // it is ambiguous whether this means the refernce type, or a reference to
    // the type object...
    b: &C2,
);

class C2();

```

```{code-block} bondrewd
:caption: "Immediate impls"

// These are perfectly okay without any special grammar needed:
impl class Foo(
    int32 a,
    int32 b,
) {
    // ...
};

impl ns std::something {
    // ...
};

// However, it gets a bit messy with templates:
// (We need to specify template parameters thrice!)
impl[T: type] class Foo[T: type]()[T] {
    // ...
};

// I need some better solution for this...
// Actually, even without immediate impl, this is still problematic
// Maybe I should allow to somehow provide an impl for a template type
// without explicitly redeclaring the template parameters?
// I certainly don't want to repeat Rust's practice of having to repeat
// all type constraints every time...

```

```{code-block} bondrewd
:caption: "Explicit template declarations"

// Class template
template[T: type] class Foo(
    // ...
);

// Impl template
template[T: type] impl Foo {
    // ...
};

// Function template
template[T: type] func bar() => {
    // ...
};

// Inline class impl template
// (No grammatical exception needed here. All this does is declare a template
//  within which lays an inline impl (impl with a class declaration inside).
//  Since impl blocks return the class object, this makes the class templated
//  automatically.)
template[T: type] impl class Baz() {
    // ...
};

// Could we use any expression here?
Abomination = template[T: type] (T, int32);

// I guess we could allow explicit names instead:
template Abomination[T: type] (T, int32);

// ... and deduce them from the object, if omitted:
template[T: type] class Smth();

// But then weird usecases arise:
template[T: type] std::int32;
// It already exists, but from the template's perspective, it's just a class
// with a qualname like any other. So, would it overwrite std::int32?
// And should it?

// What if we name the outermost entity?
template Smth2[T: type] class ();

// Maybe demand a => like with functions?
template Smth3[T: type] => impl class () {};

// This looks horrible, though...
template do_smth4[T: type] => func () => {};

```

```{code-block} bondrewd
:caption: "Reference implementation for the core traits"

impl ns std {

// A type is, essentially, just a marker trait. Specific type-related behavior
// is implemented through different traits. The primary reason for this to exist
// is that to impl object a for object b, Trait must be implemented for a's
// type, and Type should be implemented for b's type.
trait Type: Hash + Eq + Copy {
};

trait Trait {
    func get_impl_for(&self, type: &Type): Option[TraitImpl[Self]];

    func get_slots(&self): TraitSlots;
};

trait ManualTrait : Trait {
    func do_impl_for(&self, type: &Type, impl: TraitImpl[Self]): Unit;
};

// TODO: TraitImpl, TraitSlots, ...?

}

```

```{code-block} bondrewd
:caption: "Potential syntax for file-level configuration"

// 1. A file statement with some attributes
@std::use_reference_type(std::permission_ref)
file;

// 2. A file block?
file (
    reference_type = std::permission_ref;
);

// 3. 'Outer' attributes?
@!std::use_reference_type(std::permission_ref);
// or
@^std::use_reference_type(std::permission_ref);

// Note: I'd potentially like these to be able to influence even the parser
// used for the file, but I'm not sure how that should work across different
// syntaxes...

```
