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