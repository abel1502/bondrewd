"""
The C++ code generator itself.

Based on CPython's Tools/peg_generator/pegen/c_generator.py,
but reworked significantly to produce Bondrewd-compatible C++ code.
"""

from __future__ import annotations

import typing
import pathlib
import re
from dataclasses import dataclass, field
from ast import literal_eval
import enum

from pegen import grammar
from pegen.grammar import (
    Alt,
    Cut,
    Forced,
    Gather,
    GrammarVisitor,
    Group,
    Leaf,
    Lookahead,
    NamedItem,
    NameLeaf,
    NegativeLookahead,
    Opt,
    PositiveLookahead,
    Repeat0,
    Repeat1,
    Rhs,
    Rule,
    StringLeaf,
)
from pegen.parser_generator import ParserGenerator


# TODO: Implement the following:
#       - `defer` helper
#       - result_cast?
HPP_PREFIX = """\
#include <bondrewd/parser/parser_base.hpp>


#pragma region Helpers
namespace {


// TODO


}  // namespace
#pragma endregion Helpers


"""


TOKEN_TYPES: typing.Final[typing.Set[str]] = {
    "endmarker", "name", "number", "string", "keyword", "punct",
}


@dataclass
class FunctionCall:
    function: str
    arguments: typing.List[typing.Any] = field(default_factory=list)
    assigned_variable: str | None = None
    force_true: bool = False
    comment: str | None = None
    
    def get_line_comment(self) -> str:
        return f"  // {self.comment}" if self.comment else ""

    def get_cond(self) -> str:
        if self.force_true:
            return "true"
        
        if self.assigned_variable:
            return f"{self.assigned_variable}"
        
        assert False, "No condition for this call!"
    
    def get_full_call(self) -> str:
        assert self.assigned_variable, "No assigned variable!"
        
        return f"auto {self.assigned_variable} = {self};"

    def __str__(self) -> str:
        parts: typing.List[str] = []
        parts.append(self.function)
        parts.append(f"({', '.join(map(str, self.arguments))})")
        if self.assigned_variable:
            parts = [self.assigned_variable, " = "] + parts
        return "".join(parts)


class CXXCallMakerVisitor(GrammarVisitor):
    gen: CXXParserGenerator
    # Puncts and hard keywords.
    # Lookup from string to qualified name
    # (ex.: "=" -> "Punct::EQUAL")
    string_tokens: typing.Dict[str, str]
    cache: typing.Dict[typing.Any, FunctionCall]
    cleanup_statements: typing.List[str]
    
    def __init__(
        self,
        parser_generator: CXXParserGenerator,
        string_tokens: typing.Dict[str, str],
    ):
        self.gen = parser_generator
        self.string_tokens = string_tokens
        self.cache = {}
        self.cleanup_statements = []

    def string_token_helper(self, raw_value: str) -> FunctionCall:
        """
        Helper for keywords (hard and soft) and puncts parsins.
        """
        
        value: str = literal_eval(raw_value)
        
        if not re.match(r"[a-zA-Z_]\w*\Z", value):  # punct
            assert value in self.string_tokens, f"Not a valid punct: {raw_value}"
            
            return FunctionCall(
                assigned_variable="_literal",
                function=f"lexer.expect().punct",
                arguments=[self.string_tokens[value]],
                comment=f"punct={value!r}",
            )
        
        if raw_value.endswith("'"):  # hard keyword
            assert value in self.string_tokens, f"Not a valid hard keyword: {raw_value}"
            
            return FunctionCall(
                assigned_variable="_keyword",
                function="lexer.expect().keyword",
                arguments=[self.string_tokens[value]],
                comment=f"keyword={value!r}",
            )
        
        # soft keyword
        assert value not in self.string_tokens, f"Not a valid soft keyword: {raw_value}"
        
        return FunctionCall(
            assigned_variable="_keyword",
            function="lexer.expect().soft_keyword",
            arguments=[raw_value],
            comment=f"soft_keyword={raw_value!r}",
        )
    
    def complex_rule_helper(self, node: Repeat0 | Repeat1 | Gather) -> FunctionCall:
        if node in self.cache:
            return self.cache[node]
        
        name: str
        if isinstance(node, Repeat0):
            name = self.gen.name_loop(node.node, False)
        elif isinstance(node, Repeat1):
            name = self.gen.name_loop(node.node, True)
        elif isinstance(node, Gather):
            name = self.gen.name_gather(node)
        else:
            assert False, f"Not a complex rule: {node}"
        
        self.cache[node] = FunctionCall(
            assigned_variable=f"{name}_var",
            function=f"parse_{name}_rule",
            arguments=[],
            comment=f"{node}",
        )
        
        return self.cache[node]
    
    @staticmethod
    def can_rhs_be_inlined(rhs: Rhs) -> bool:
        if len(rhs.alts) != 1 or len(rhs.alts[0].items) != 1:
            return False
        
        # If the alternative has an action we cannot inline
        return getattr(rhs.alts[0], "action", None) is None

    def visit_NameLeaf(self, node: NameLeaf) -> FunctionCall:
        name: str = node.value
        if name.isupper() and name.lower() in TOKEN_TYPES:
            return FunctionCall(
                assigned_variable=f"{name}_var",
                function=f"lexer.expect().token",
                arguments=[f"TokenType::{name}"],
                comment=f"token={name}",
            )

        rule: Rule = self.gen.all_rules.get(name.lower())
        # TODO: Was ignored originally
        assert rule is not None, f"Rule {name} not found"
        
        return FunctionCall(
            assigned_variable=f"{name}_var",
            function=f"parse_{name}_rule",
            arguments=[],
            comment=f"{node}",
        )

    def visit_StringLeaf(self, node: StringLeaf) -> FunctionCall:
        return self.string_token_helper(node.value)

    def visit_Rhs(self, node: Rhs) -> FunctionCall:
        if node in self.cache:
            return self.cache[node]
        
        if self.can_rhs_be_inlined(node):
            self.cache[node] = self.generate_call(node.alts[0].items[0])
            return self.cache[node]
        
        name: str = self.gen.name_node(node)
        self.cache[node] = FunctionCall(
            assigned_variable=f"{name}_var",
            function=f"parse_{name}_rule",
            arguments=[],
            comment=f"{node}",
        )
        return self.cache[node]

    def visit_NamedItem(self, node: NamedItem) -> FunctionCall:
        call: FunctionCall = self.generate_call(node.item)
        if node.name:
            call.assigned_variable = node.name
        return call

    def lookahead_call_helper(self, node: Lookahead, positive: bool) -> FunctionCall:
        call: FunctionCall = self.generate_call(node.node)
        
        positive_str: str = "true" if positive else "false"

        # if isinstance(node.node, Leaf):
        if call.function.startswith("lexer.expect()."):
            call.function = call.function.replace(
                "lexer.expect().",
                f"lexer.lookahead({positive_str}).",
                1
            )
            call.assigned_variable = None
            
            return call
        
        # TODO: More neat solution?
        # Always has to capture `this`, if nothing else
        call.assigned_variable = None
        lambda_call = f"[&]() {{ return {call}; }}"
        
        return FunctionCall(
            function=f"lookahead",
            arguments=[positive_str, lambda_call],
            comment=f"lookahead: {call.comment}"
        )

    def visit_PositiveLookahead(self, node: PositiveLookahead) -> FunctionCall:
        return self.lookahead_call_helper(node, True)

    def visit_NegativeLookahead(self, node: NegativeLookahead) -> FunctionCall:
        return self.lookahead_call_helper(node, False)

    def visit_Forced(self, node: Forced) -> FunctionCall:
        call: FunctionCall = self.generate_call(node.node)
        
        call.assigned_variable = None
        
        return FunctionCall(
            function=f"forced",
            arguments=[str(call)],
            comment=f"forced: {call.comment}"
        )

    def visit_Opt(self, node: Opt) -> FunctionCall:
        call: FunctionCall = self.generate_call(node.node)
        
        call.assigned_variable = "_opt_var"
        call.force_true = True
        call.comment = f"{node}"
        
        return call

    def visit_Repeat0(self, node: Repeat0) -> FunctionCall:
        return self.complex_rule_helper(node)

    def visit_Repeat1(self, node: Repeat1) -> FunctionCall:
        return self.complex_rule_helper(node)

    def visit_Gather(self, node: Gather) -> FunctionCall:
        return self.complex_rule_helper(node)

    def visit_Group(self, node: Group) -> FunctionCall:
        return self.generate_call(node.rhs)

    def visit_Cut(self, node: Cut) -> FunctionCall:
        # TODO: ?
        return FunctionCall(
            assigned_variable="_cut_var",
            function="std::optional<int>",
            arguments=["1"],
        )

    def generate_call(self, node: typing.Any) -> FunctionCall:
        return self.visit(node)


class CXXParserGenerator(ParserGenerator, GrammarVisitor):
    # TODO: keywords, soft_keywords, puncts
    pass
