Grammar specification
=====================

This document contains the most up-to-date grammar specification for the
Bondrewd programming language. The grammar is written in a modified version of
[Pegen](https://we-like-parsers.github.io/pegen/)'s format. The parser produced
is a PEG parser with packrat caching.

The lexical specification is still to be documented.

```{note}
The grammar is still a work in progress. Use for general reference only.
```

## Grammar
```{literalinclude} ../../grammar/bondrewd.gram
:linenos:
```
