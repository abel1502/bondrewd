Compile-time model
==================

One of the primary ideas of the language is to describe much of it as
compile-time entities, as opposed to language-level features. To support that,
the language should provide a powerful compile-time programming model. This
document aims to describe it in detail.

## `ctime` functions

You might be familiar with C++'s `constexpr` and `consteval` function modifiers.
In Bondrewd, there's a similar keyword: `ctime`. Let us clarify its meaning:
 - Any function may be called in a compile-time expression. The compiler will
    try to evaluate it at compile-time, and if it succeeds, it will be replaced
    with the result of the evaluation. If it fails, you will see a compilation
    error. This behaviour is similar to `constexpr` in C++, but requires no
    explicit marking of functions.
    ```{code-block} bondrewd
    :caption: "Example: regular function evaluated at compile time"
    // Works regardless of whether `sqr` is `ctime` or not
    var arr: int32[sqr(3)];
    ```
    <!-- TODO: bondrewd syntax highlighting -->
 - A `ctime` function (one whose body is marked as a `ctime`-expression)
    cannot be called at runtime. All invocations of it must be resolvable at
    compile time. This is similar to `consteval` in C++. Unlike C++, however,
    `ctime` functions gains access to a much wider range of language features
    related to metaprogramming.
    ```{code-block} bondrewd
    :caption: "Example: `ctime` function cannot be called at runtime"
    var n = int32.parse(input());
    // If `foo` is `ctime`, this will fail to compile
    foo(n);
    ```

## Other `ctime` uses
That being said, `ctime` isn't exclusively about functions. You can use the
`ctime` keyword in the following cases:
 - With a code block:
    ```{code-block} bondrewd
    ctime {
        // Some code
    };
    ```
    This is actually the syntax used to define `ctime` functions as well.
 - With a variable declaration:
    ```{code-block} bondrewd
    ctime var x = 5;
    ```
 - With a class declaration:
    ```{code-block} bondrewd
    ctime class Foo {
        // ...
    };
    ```
 - With a trait declaration:
    ```{code-block} bondrewd
    ctime trait Foo {
        // ...
    };
    ```

## `ctime` contexts
A `ctime` context is a block of code that must be evaluated entirely at
compile-time. You've already met some example in this document. Here are the
rest of them:
 - Type annotations
 - `ctime` code blocks (including `ctime` function bodies)
 - `ctime` variable initializers
 - Arguments to `ctime` function calls (including all template arguments, since
    templates are just overloaded square brackets)
 - ... (TODO: List the rest)
