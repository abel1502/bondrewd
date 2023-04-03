Compile-time model
==================

One of the primary ideas of the language is to describe much of it as
compile-time entities, as opposed to language-level features. To support that,
the language should provide a powerful compile-time metaprogramming. This
document aims to describe it in detail.

## Introduction
To better understand the operation of Bondrewd's compile-time, think of your
program not as of a definition in a static format, but as a script responsible
for generating an executable. It has the compiler at its disposal, utilizing it
as a helper library. This 'compilation script' is, overall, an imperative
program, but you have a lot of declarative constructs at your disposal.

What is different from the 'script and library' scenario, and what might cause
some confusion at first, is that the language of the 'script' and the language
used for the declarative constructs largely coincide, and even interoperate to
an extent. However, in exchange this provides a lot of power and flexibility.

## Compile- and runtime scopes
At the top level, your program starts with compile-time code. Any class,
function, or variable definition is always a compile-time statement.
The only places where runtime code is allowed (runtime scopes) are
inside a runtime function and in the initializer of a runtime variable.

```{note}
A function is runtime by default (implicitly) or if it has the `rtime` keyword.
To make a function compile-time, use the `ctime` keyword.
```

In a compile-time scope, all code must be executed at compile-time. In a runtime
scope, some code may be executed at compile-time, and some at runtime. This
involves, for example:
 - Constant-folding (e.g. `1 + 2` may be evaluated to `3` at compile-time);
 - Type annotations (e.g. in `var foo: int32`, `int32` is a compile-time
    entity);
 - Explicitly `ctime` code (e.g. `ctime { ... }`, `ctime func foo() { ... }`, 
    `ctime var bar = 42`, `ctime if (true) { ... }`, etc.);
 - Behavior of some code can be redefined at compile-time (e.g. attribute
    resolution, operator overloading, etc.).

The most important thing to remember is that a runtime function is merely a
compile-time object containing an AST of runtime code and some metadata. Its
compilation is handled, to an extent, as an ordinary compile-time operation,
which means it may be influenced by compile-time code. Pretty much the same
holds for runtime variables.

## Execution order
Compile-time code is executed in the direct, straight-forward order. No forward
references are available (because there's no feasible way to continue compiling
until their definition without a preceding statement). Think of it like what
Python does. This also means that from within functions, you may use yet
undefined items, so long as they are defined by the time the function is first
called. This holds for both compile-time and runtime functions, with the catch
that runtime functions aren't 'called' at compile-time, but are 'compiled' at
some point. For them, the 'first call' is the first (and only) time they are
compiled. Usually, this is one of the last phases of compilation, so you may
mostly rely on all compile-time definitions being available. Note, however, that
an unused runtime function may never be compiled, so you should not rely on
side effects of compile-time code in its definition.
