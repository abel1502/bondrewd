#-------------------------------------------------------------------------------
# Parser for ASDL [1] definition files. Reads in an ASDL description and parses
# it into an AST that describes it.
#
# The EBNF we're parsing here: Figure 1 of the paper [1]. Extended to support
# modules and attributes after a product. Words starting with Capital letters
# are terminals. Literal tokens are in "double quotes". Others are
# non-terminals. Id is either TokenId or ConstructorId.
#
# module        ::= "module" Id "{" [definitions] "}"
# definitions   ::= { TypeId "=" type }
# type          ::= product | sum
# product       ::= fields ["attributes" fields]
# fields        ::= "(" { field, "," } field ")"
# field         ::= TypeId ["?" | "*"] [Id]
# sum           ::= constructor { "|" constructor } ["attributes" fields]
# constructor   ::= ConstructorId [fields]
#
# [1] "The Zephyr Abstract Syntax Description Language" by Wang, et. al. See
#     http://asdl.sourceforge.net/
#-------------------------------------------------------------------------------
from __future__ import annotations
import typing
from collections import namedtuple
import re
from os import PathLike
import enum
import dataclasses


__all__ = [
    "builtin_types", "parse", "AST", "Module", "Type", "Constructor",
    "Field", "Sum", "Product", "VisitorBase", "Check", "check"
]


# The following classes define nodes into which the ASDL description is parsed.
# Note: this is a "meta-AST". ASDL files (such as Python.asdl) describe the AST
# structure used by a programming language. But ASDL files themselves need to be
# parsed. This module parses ASDL files and uses a simple AST to represent them.
# See the EBNF at the top of the file to understand the logical connection
# between the various node types.

# Updates from the caller
default_builtin_types = {
    "identifier", "int", "string", "constant",
}


class AST:
    def __repr__(self):
        raise NotImplementedError


class Module(AST):
    name: str
    dfns: typing.List[Type]
    types: typing.Dict[str, Type]
    
    def __init__(self, name: str, dfns: typing.List[Type]):
        self.name = name
        self.dfns = dfns
        self.types = {type.name: type.value for type in dfns}

    def __repr__(self):
        return f"Module({self.name}, {self.dfns})"


class Type(AST):
    name: str
    value: Sum | Product
    
    def __init__(self, name: str, value: Sum | Product):
        self.name = name
        self.value = value

    def __repr__(self):
        return f"Type({self.name}, {self.value})"


class Constructor(AST):
    name: str
    fields: typing.List[Field]
    
    def __init__(self, name: str, fields: typing.List[Field] | None = None):
        self.name = name
        self.fields = fields or []

    def __repr__(self):
        return f"Constructor({self.name}, {self.fields})"


class Field(AST):
    type: str
    name: str | None
    seq: bool
    opt: bool
    
    def __init__(self,
                 type: str,
                 name: str | None = None,
                 seq: bool = False,
                 opt: bool = False):
        self.type = type
        self.name = name
        self.seq = seq
        self.opt = opt

    def __str__(self):
        if self.seq:
            extra = "*"
        elif self.opt:
            extra = "?"
        else:
            extra = ""

        return f"{self.type}{extra} {self.name}"

    def __repr__(self):
        extra: str
        if self.seq:
            extra = ", seq=True"
        elif self.opt:
            extra = ", opt=True"
        else:
            extra = ""
        
        if self.name is not None:
            extra = f", {self.name}{extra}"
            
        return f"Field({self.type}{extra})"


class Sum(AST):
    types: typing.List[Constructor]
    attributes: typing.List[Field]
    
    def __init__(self, types: typing.List[Constructor], attributes: typing.List[Field] | None = None):
        self.types = types
        self.attributes = attributes or []

    def __repr__(self):
        return f"Sum({self.types}{', ' if self.attributes else ''}{self.attributes or ''})"


class Product(AST):
    fields: typing.List[Field]
    attributes: typing.List[Field]
    
    def __init__(self, fields: typing.List[Field], attributes: typing.List[Field] | None = None):
        self.fields = fields
        self.attributes = attributes or []

    def __repr__(self):
        return f"Product({self.fields}{', ' if self.attributes else ''}{self.attributes or ''})"


# A generic visitor for the meta-AST that describes ASDL. This can be used by
# emitters. Note that this visitor does not provide a generic visit method, so a
# subclass needs to define visit methods from visitModule to as deep as the
# interesting node.
# We also define a Check visitor that makes sure the parsed ASDL is well-formed.
class VisitorBase(object):
    """ Generic tree visitor for ASTs. """
    
    cache: typing.Dict[typing.Type[AST], typing.Callable]
    
    def __init__(self):
        self.cache = {}

    def visit(self, obj: AST, *args) -> None:
        klass = type(obj)
        meth = self.cache.get(klass)
        
        if meth is None:
            methname = "visit" + klass.__name__
            meth = getattr(self, methname, None)
            self.cache[klass] = meth
        
        if meth:
            try:
                meth(obj, *args)
            except Exception as e:
                print(f"Error visiting {obj!r}: {e}")
                raise


class Check(VisitorBase):
    """
    A visitor that checks a parsed ASDL tree for correctness.

    Errors are printed and accumulated.
    """
    
    cons: typing.Dict[str, str]
    errors: int
    types: typing.Dict[str, Type]
    
    def __init__(self):
        super().__init__()
        self.cons = {}
        self.errors = 0
        self.types = {}

    def visitModule(self, mod: Module) -> None:
        for dfn in mod.dfns:
            self.visit(dfn)

    def visitType(self, type: Type) -> None:
        self.visit(type.value, str(type.name))

    def visitSum(self, sum: Sum, name: str) -> None:
        for t in sum.types:
            self.visit(t, name)

    def visitConstructor(self, cons: Constructor, name: str) -> None:
        key: str = str(cons.name)
        conflict: str | None = self.cons.get(key)
        
        if conflict is None:
            self.cons[key] = name
        else:
            print(f"Redefinition of constructor {key}")
            print(f"Defined in {conflict} and {name}")
            self.errors += 1
        
        for f in cons.fields:
            self.visit(f, key)

    def visitField(self, field: Field, name: str) -> None:
        key: str = str(field.type)
        l: typing.List[Type] = self.types.setdefault(key, [])
        l.append(name)

    def visitProduct(self, prod: Product, name: str) -> None:
        for f in prod.fields:
            self.visit(f, name)


def check(mod: Module, builtin_types: typing.Container[str] = default_builtin_types) -> bool:
    """
    Check the parsed ASDL tree for correctness.

    Return True if success. For failure, the errors are printed out and False
    is returned.
    """
    
    v = Check()
    v.visit(mod)

    for t in v.types:
        if t not in mod.types and not t in builtin_types:
            v.errors += 1
            uses = ", ".join(v.types[t])
            print(f"Undefined type {t}, used in {uses}")
    
    return not v.errors


# The ASDL parser itself comes next. The only interesting external interface
# here is the top-level parse function.
def parse(filename: PathLike) -> Module:
    """ Parse ASDL from the given file and return a Module node describing it. """
    
    with open(filename, encoding="utf-8") as f:
        parser = ASDLParser()
        data = f.read()
    
    return parser.parse(data)


# Types for describing tokens in an ASDL specification.
class TokenKind(enum.Enum):
    """ TokenKind is provides a scope for enumerated token kinds. """
    
    ConstructorId = enum.auto()
    TypeId = enum.auto()
    Equals = enum.auto()
    Comma = enum.auto()
    Question = enum.auto()
    Pipe = enum.auto()
    Asterisk = enum.auto()
    LParen = enum.auto()
    RParen = enum.auto()
    LBrace = enum.auto()
    RBrace = enum.auto()


TokenKind.operator_table: typing.Mapping[str, TokenKind] = {
    "=": TokenKind.Equals, ",": TokenKind.Comma,    "?": TokenKind.Question, "|": TokenKind.Pipe,  "(": TokenKind.LParen,
    ")": TokenKind.RParen, "*": TokenKind.Asterisk, "{": TokenKind.LBrace,   "}": TokenKind.RBrace
}


@dataclasses.dataclass(frozen=True)
class Token:
    kind: TokenKind
    value: str
    lineno: int


class ASDLSyntaxError(Exception):
    msg: str
    lineno: int | str
    
    def __init__(self, msg: str, lineno: int | str | None = None):
        self.msg = msg
        self.lineno = lineno or "<unknown>"

    def __str__(self):
        return f"Syntax error on line {self.lineno}: {self.msg}"


def tokenize_asdl(buf: str) -> typing.Generator[Token, None, None]:
    """ Tokenize the given buffer. Yield Token objects. """
    
    for lineno, line in enumerate(buf.splitlines(), 1):
        for m in re.finditer(r"\s*(\w+|--.*|.)", line.strip()):
            c = m.group(1)
            if c[0].isalpha():
                # Some kind of identifier
                yield Token(TokenKind.ConstructorId if c[0].isupper() else TokenKind.TypeId, c, lineno)
            elif c[:2] == "--":
                # Comment
                break
            else:
                # Operators
                try:
                    op_kind = TokenKind.operator_table[c]
                except KeyError:
                    raise ASDLSyntaxError(f"Invalid operator {c}", lineno)
                yield Token(op_kind, c, lineno)


class ASDLParser:
    """
    Parser for ASDL files.

    Create, then call the parse method on a buffer containing ASDL.
    This is a simple recursive descent parser that uses tokenize_asdl for the
    lexing.
    """
    
    _tokenizer: typing.Generator[Token, None, None] | None
    cur_token: Token | None
    
    def __init__(self):
        self._tokenizer = None
        self.cur_token = None

    def parse(self, buf: str) -> Module:
        """ Parse the ASDL in the buffer and return an AST with a Module root. """
        self._tokenizer = tokenize_asdl(buf)
        self._advance()
        return self._parse_module()

    def _parse_module(self) -> Module:
        if self._at_keyword("module"):
            self._advance()
        else:
            raise ASDLSyntaxError(
                f"Expected \"module\" (found {self.cur_token.value})",
                self.cur_token.lineno
            )
        name = self._match(self._id_kinds)
        self._match(TokenKind.LBrace)
        defs = self._parse_definitions()
        self._match(TokenKind.RBrace)
        return Module(name, defs)

    def _parse_definitions(self) -> typing.List[Type]:
        defs: typing.List[Type] = []
        while self.cur_token.kind == TokenKind.TypeId:
            typename = self._advance()
            self._match(TokenKind.Equals)
            type = self._parse_type()
            defs.append(Type(typename, type))
        return defs

    def _parse_type(self) -> Type:
        if self.cur_token.kind == TokenKind.LParen:
            # If we see a (, it's a product
            return self._parse_product()
        else:
            # Otherwise it's a sum. Look for ConstructorId
            sumlist = [Constructor(
                self._match(TokenKind.ConstructorId),
                self._parse_optional_fields()
            )]
            while self.cur_token.kind  == TokenKind.Pipe:
                # More constructors
                self._advance()
                sumlist.append(
                    Constructor(
                        self._match(TokenKind.ConstructorId),
                        self._parse_optional_fields()
                    )
                )
            return Sum(sumlist, self._parse_optional_attributes())

    def _parse_product(self) -> Product:
        return Product(self._parse_fields(), self._parse_optional_attributes())

    def _parse_fields(self) -> typing.List[Field]:
        fields: typing.List[Field] = []
        self._match(TokenKind.LParen)
        while self.cur_token.kind == TokenKind.TypeId:
            typename = self._advance()
            is_seq, is_opt = self._parse_optional_field_quantifier()
            id = (self._advance() if self.cur_token.kind in self._id_kinds
                                  else None)
            fields.append(Field(typename, id, seq=is_seq, opt=is_opt))
            if self.cur_token.kind == TokenKind.RParen:
                break
            if self.cur_token.kind == TokenKind.Comma:
                self._advance()
        self._match(TokenKind.RParen)
        return fields

    def _parse_optional_fields(self) -> typing.List[Field] | None:
        if self.cur_token.kind == TokenKind.LParen:
            return self._parse_fields()
        return None

    def _parse_optional_attributes(self) -> typing.List[Field] | None:
        if self._at_keyword("attributes"):
            self._advance()
            return self._parse_fields()
        return None

    def _parse_optional_field_quantifier(self) -> typing.Tuple[bool, bool]:
        is_seq, is_opt = False, False
        if self.cur_token.kind == TokenKind.Asterisk:
            is_seq = True
            self._advance()
        elif self.cur_token.kind == TokenKind.Question:
            is_opt = True
            self._advance()
        return is_seq, is_opt

    def _advance(self) -> str | None:
        """ 
        Return the value of the current token and read the next one into
        self.cur_token.
        """
        cur_val = None if self.cur_token is None else self.cur_token.value
        try:
            self.cur_token = next(self._tokenizer)
        except StopIteration:
            self.cur_token = None
        return cur_val

    _id_kinds: typing.Tuple[TokenKind, ...] = (TokenKind.ConstructorId, TokenKind.TypeId)

    def _match(self, kind: TokenKind | typing.Tuple[TokenKind, ...]) -> str:
        """
        The 'match' primitive of RD parsers.

        * Verifies that the current token is of the given kind (kind can
          be a tuple, in which the kind must match one of its members).
        * Returns the value of the current token
        * Reads in the next token
        """
        if (isinstance(kind, tuple) and self.cur_token.kind in kind or
            self.cur_token.kind == kind):
            
            value = self.cur_token.value
            self._advance()
            return value
        
        raise ASDLSyntaxError(
            f"Unmatched {kind} (found {self.cur_token.kind})",
            self.cur_token.lineno
        )

    def _at_keyword(self, keyword: str) -> bool:
        return (
            self.cur_token.kind == TokenKind.TypeId and
            self.cur_token.value == keyword
        )
