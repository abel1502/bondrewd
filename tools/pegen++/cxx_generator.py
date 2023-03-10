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
from contextlib import contextmanager
from textwrap import dedent
import functools
import io

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
    tokens_to_names: typing.Dict[str, str]
    cache: typing.Dict[typing.Any, FunctionCall]
    
    def __init__(
        self,
        parser_generator: CXXParserGenerator,
        string_tokens: typing.Dict[str, str],
    ):
        self.gen = parser_generator
        self.tokens_to_names = string_tokens
        self.cache = {}

    def string_token_helper(self, raw_value: str) -> FunctionCall:
        """
        Helper for keywords (hard and soft) and puncts parsins.
        """
        
        value: str = literal_eval(raw_value)
        
        if not re.match(r"[a-zA-Z_]\w*\Z", value):  # punct
            assert value in self.tokens_to_names, f"Not a valid punct: {raw_value}"
            
            return FunctionCall(
                assigned_variable="_literal",
                function=f"lexer.expect().punct",
                arguments=[self.tokens_to_names[value]],
                comment=f"punct={value!r}",
            )
        
        if raw_value.endswith("'"):  # hard keyword
            assert value in self.tokens_to_names, f"Not a valid hard keyword: {raw_value}"
            
            return FunctionCall(
                assigned_variable="_keyword",
                function="lexer.expect().keyword",
                arguments=[self.tokens_to_names[value]],
                comment=f"keyword={value!r}",
            )
        
        # soft keyword
        assert value not in self.tokens_to_names, f"Not a valid soft keyword: {raw_value}"
        
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
            function="std::optional<std::monostate>",
            arguments=["std::monostate{}"],
        )

    def generate_call(self, node: typing.Any) -> FunctionCall:
        return self.visit(node)


class CXXTypeDeductionVisitor(GrammarVisitor):
    # TODO: Also unwrap quoted types!
    # TODO: Also deduce types for alts with a single nonterminal and no actions
    
    pass  # TODO: Implement


P = typing.ParamSpec("P")


def jinja_tpl_generator(func: typing.Callable[P, None]) -> typing.Callable[P, str]:
    @functools.wraps(func)
    def wrapper(self: CXXParserGenerator, *args, **kwargs) -> str:
        # As a force reset
        # self.get_contents()
        
        func(self, *args, **kwargs)
        
        return self.get_contents()
    
    return wrapper


class CXXParserGenerator(ParserGenerator, GrammarVisitor):
    callmakervisitor: CXXCallMakerVisitor
    debug: bool
    skip_actions: bool
    _varname_counter: int
    file: io.StringIO
    
    def __init__(
        self,
        grammar: grammar.Grammar,
        tokens_to_names: typing.Dict[str, str],  # keywords and puncts
        debug: bool = False,
        skip_actions: bool = False,
    ):
        ParserGenerator.__init__(self, grammar, TOKEN_TYPES, io.StringIO())
        GrammarVisitor.__init__(self)
        
        self.callmakervisitor = CXXCallMakerVisitor(self, tokens_to_names)
        self._varname_counter = 0
        self.debug = debug
        self.skip_actions = skip_actions

    def get_contents(self) -> str:
        result: str = self.file.getvalue()
        
        self.file = io.StringIO()
        
        return result
    
    def add_level(self) -> None:
        pass  # TODO: Maybe implement for recursion level tracking?

    def remove_level(self) -> None:
        pass

    def add_return(self, ret_val: str) -> None:
        self.remove_level()
        self.print(f"return {ret_val};")

    def unique_varname(self, name: str = "tmpvar") -> str:
        new_var = name + "_" + str(self._varname_counter)
        self._varname_counter += 1
        return new_var
    
    @contextmanager
    def if_impl(self, with_braces: bool = False) -> typing.Generator[None, None, None]:
        self.print("#ifdef BONDREWD_PARSER_IMPL")
        if with_braces:
            self.print("{")
            with self.indent():
                yield
            self.print("}")
        else:
            yield
        self.print("#endif")

    def generate(self, filename: str) -> None:
        raise NotImplementedError("Use prepare() and a jinja template instead")
    
    def prepare(self) -> CXXParserGenerator:
        self.collect_todo()
        
        if "header" in self.grammar.metas:
            raise NotImplementedError("Overrding header not supported anymore")
        
        # TODO: Compute all rule types!
        
        # TODO: Mark all loops for caching?
        
        return self

    @functools.cached_property
    def all_rules_sorted(self) -> typing.List[typing.Tuple[str, Rule]]:
        return list(sorted(self.all_rules.items(), key=lambda x: x[0]))

    @jinja_tpl_generator
    def gen_subheader(self) -> None:
        subheader = self.grammar.metas.get("subheader", "")
        if subheader:
            self.print(subheader)

    @jinja_tpl_generator
    def gen_api(self) -> None:
        if "start" not in self.all_rules:
            # This is somewhat counter-intuitive, but this is what the trailer is supposed to be for
            trailer: str = self.grammar.metas.get("trailer", "")
            
            assert trailer, "No start rule and no trailer"
            
            self.print(trailer)
            
            return
        
        self.print("auto parse() {")
        with self.indent():
            self.print("return parse_start();")
        self.print("}")
    
    @jinja_tpl_generator
    def gen_rule_parsers(self) -> None:
        for rule in list(self.all_rules.values()):
            self.print()
            if rule.left_recursive:
                self.print("// Left-recursive")
            self.visit(rule)

    def should_cache(self, node: Rule) -> bool:
        return node.memo and not node.left_recursive
    
    def visit_Rule(self, node: Rule) -> None:
        is_loop = node.is_loop()
        is_gather = node.is_gather()
        rhs = node.flatten()

        for line in str(node).splitlines():
            self.print(f"// {line}")
        
        if node.left_recursive and node.leader:
            self.print(f"std::optional<{node.type}> parse_raw_{node.name}();")

        self.print(f"std::optional<{node.type}> parse_{node.name}()")
        
        if node.left_recursive and node.leader:
            self._set_up_rule_caching(node)

        with self.if_impl(with_braces=True):
            with self.indent():
                if is_loop:
                    self._handle_loop_rule_body(node, rhs)
                else:
                    self._handle_default_rule_body(node, rhs)
    
    def _set_up_rule_caching(self, node: Rule) -> None:
        with self.if_impl(with_braces=True):
            assert node.type is not None, "All rules must have specific types!"
            result_type: str = node.type
            
            self.add_level()
            self.print(f"std::optional<{result_type}> _res = std::nullopt;")
            # TODO: Guard for saving _res in case of success
            self.print(f"if (_res = get_cached<RuleType::{node.name}>(_state)) {{")
            with self.indent():
                self.add_return("_res")
            self.print("}")
            
            self.print("auto _state = tell();")
            self.print("auto _res_state = tell();")
            
            self.print("while (true) {")
            with self.indent():
                self.print(f"store_cached<RuleType::{node.name}>(_state, _res);")
                self.print("seek(_state);")
                self.print(f"auto _raw = parse_raw_{node.name}();")
                self.print("if (!_raw || tell() <= _res_state) {")
                with self.indent():
                    self.print("break;")
                self.print("}")
                self.print("_res = _raw;")
                self.print("_res_state = tell();")
            self.print("}")
            
            self.print("seek(_res_state);")
            self.add_return("_res")
        
        self.print(f"std::optional<{node.type}> parse_raw_{node.name}()")
        
    def _handle_default_rule_body(self, node: Rule, rhs: Rhs) -> None:
        should_cache: bool = self.should_cache(node)
        
        assert node.type is not None, "All rules must have specific types!"
        result_type: str = node.type

        self.add_level()
        self.print(f"std::optional<{result_type}> _res = std::nullopt;")
        
        if should_cache:
            self.print(f"if (_res = get_cached<RuleType::{node.name}>(_state)) {{")
            with self.indent():
                self.add_return("_res")
            self.print("}")
            
            # If `cache_guard.accept()` is called, the cache is automatically
            # set upon leaving the function scope
            self.print(f"CacheHelper<RuleType::{node.name}> cache_guard{{*this, _state, _res}};")
        
        # TODO: Pass the type as well?
        self.visit(
            rhs,
            is_loop=False,
            is_gather=node.is_gather(),
            rulename=node.name,
            should_cache=should_cache,
        )
        
        self.print("_res = std::nullopt;")
        self.add_return("_res")

    def _handle_loop_rule_body(self, node: Rule, rhs: Rhs) -> None:
        should_cache: bool = self.should_cache(node)
        is_repeat1: bool = node.name.startswith("_loop1")
        
        # TODO: Maybe loop results are different?
        assert node.type is not None, "All rules must have specific types!"
        result_type: str = node.type
        
        assert node.name, "Unexpected unnamed loop rule!"

        self.add_level()
        self.print(f"std::optional<{result_type}> _res = std::nullopt;")
        if should_cache:
            self.print(f"if (_res = get_cached(_state, RuleType::{node.name})) {{")
            with self.indent():
                self.add_return("_res")
            self.print("}")
        self.print("auto _state = tell();")
        self.print("int _start_state = tell();")
        self.print(f"std::vector<{result_type}> _children{{}};")
        self.print("size_t _n = 0;")
        self.visit(
            rhs,
            is_loop=True,
            is_gather=node.is_gather(),
            rulename=node.name,
            should_cache=False,  # False because there shouldn't be any short-circuiting returns,
                                 # and we don't have a cache-guard because of that
        )
        
        if is_repeat1:
            self.print("if (_n == 0) {")
            with self.indent():
                self.add_return("std::nullopt")
            self.print("}")
        
        self.print("auto _seq = ast::make_sequence(std::move(_res));")
        if should_cache:
            # No cache guard used, so we store manually
            self.print(f"store_cached(_state, RuleType::{node.name}, _seq);")
        self.add_return("_seq")

    def visit_Rhs(
        self, node: Rhs, is_loop: bool, is_gather: bool, rulename: str | None, should_cache: bool
    ) -> None:
        if is_loop:
            assert len(node.alts) == 1
        
        for alt in node.alts:
            self.visit(alt, is_loop=is_loop, is_gather=is_gather, rulename=rulename, should_cache=should_cache)
    
    def visit_Alt(
        self, node: Alt, is_loop: bool, is_gather: bool, rulename: str | None, should_cache: bool
    ) -> None:
        self.print(f"{{ // {node}")
        with self.indent():
            vars = self.collect_vars(node)
            has_cut: bool = "_cut_var" in vars
            
            if has_cut:
                self.print("std::optional<std::monostate> _cut_var = std::nullopt;")

            with self.local_variable_context():
                if is_loop:
                    self.handle_alt_loop(node, rulename)
                else:
                    self.handle_alt_normal(node, is_gather, rulename, should_cache)

            self.print("seek(_state);")
            
            if "_cut_var" in vars:
                self.print("if (_cut_var) {")
                with self.indent():
                    self.add_return("std::nullopt")
                self.print("}")
        self.print("}")
    
    def handle_alt_normal(self, node: Alt, is_gather: bool, rulename: str | None, should_cache: bool) -> None:
        with self.nest_conditions(node):
            self.emit_action(node, skip=self.skip_actions, is_gather=is_gather)

            # As the current option has parsed correctly, do not continue with the rest.
            if should_cache:
                self.print("cache_guard.accept();")
            self.add_return("_res")

    def handle_alt_loop(self, node: Alt, rulename: str | None) -> None:
        self.print("while (true) {")
        with self.indent():
            with self.nest_conditions(node):
                # Loops are never gathers
                self.emit_action(node, skip=self.skip_actions, is_gather=False)

                # Add the result of rule to the temporary buffer of children. This buffer
                # will populate later an asdl_seq with all elements to return.
                self.print("_children.push_back(_res);")
                self.print("_state = tell();")
                self.print("continue;")
            self.print("break;")
        self.print("}")
    
    @contextmanager
    def nest_conditions(self, node: Alt) -> typing.Generator[None, None, None]:
        for item in node.items:
            call = self.emit_call(item)
            
            assert call.assigned_variable is not None, "Unexpected variable-less call!"
            cond: str = call.assigned_variable
            if call.force_true:
                cond = "true"
            
            self.print(f"if ({cond}) {{")
        
        with self.indent():
            yield
        
        for _ in node.items:
            self.print("}")

    def emit_action(self, node: Alt, skip: bool, is_gather: bool) -> None:
        if skip:
            self.print("_res = _dummy_whatever_();")
            return
        
        if node.action:
            self.print(f"_res = {node.action};")
            return
        
        # TODO: Horrible. Rewrite to not rely on the local variable array so much
        if len(self.local_variable_names) > 1:
            if is_gather:
                assert len(self.local_variable_names) == 2
                # TODO: Remove!
                self.print(f"// !!! DBG: emit_default_action(gather) locals: {self.local_variable_names}")
                self.print(
                    f"_res.push_back(std::move({self.local_variable_names[1]}));"
                )
            else:
                self.print(
                    f"_res = _dummy_whatever_({', '.join(self.local_variable_names)});"
                )
        else:
            self.print(f"_res = {self.local_variable_names[0]};")

    def emit_call(self, node: NamedItem) -> FunctionCall:
        call = self.callmakervisitor.generate_call(node)
        
        if call.assigned_variable == "_cut_var":
            self.print(f"_cut_var = {call};")
            return
        
        if call.assigned_variable:
            call.assigned_variable = self.dedupe(call.assigned_variable)
        
        self.print(call.get_full_call())
        
        return call

    def collect_vars(self, node: Alt) -> typing.Dict[str, str | None]:
        types = {}
        with self.local_variable_context():
            for item in node.items:
                name, type = self.add_var(item)
                if name is None:
                    continue
                types[name] = type
        return types

    def add_var(self, node: NamedItem) -> typing.Tuple[str | None, str | None]:
        call: FunctionCall = self.callmakervisitor.generate_call(node.item)
        name: str | None = node.name or call.assigned_variable
        if name is not None:
            name = self.dedupe(name)
        return_type: str | None = node.type  # or call.return_type
        return name, return_type
