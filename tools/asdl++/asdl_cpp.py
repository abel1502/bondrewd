""" Generate C++ code from an ASDL description. """
from __future__ import annotations
import typing
import argparse
import pathlib
import textwrap
import dataclasses
from ast import literal_eval
from contextlib import contextmanager

import sys
sys.path.append(str(pathlib.Path(__file__).parent.parent))
from jinja_codegen import *

import asdl


parser = argparse.ArgumentParser(
    description=textwrap.dedent("""
    The script responsible for generating several AST-related source files.
    
    More precisely, the script generates the following files:
     - include/bondrewd/ast/ast_nodes.generated.hpp
    """)
)

parser.add_argument(
    "-a", "--asdl",
    type=pathlib.Path,
    help="The path to the asdl definition file.",
    default=PROJECT_ROOT / "grammar/keywords",
)

parser.add_argument(
    "-o", "--output",
    type=pathlib.Path,
    help="The path to the root of the interpreter directory.",
    default=PROJECT_ROOT,
)


builtin_types: typing.Final[typing.Dict[str, str]] = {
    "identifier": "identifier",
    "int": "int",
    "unsigned": "unsigned",
    "bool": "bool",
    "string": "string",
    "string_view": "string_view",
    "constant": "constant",
    "SrcLocation": "lex::SrcLocation",
}


class _helpers:
    @staticmethod
    def copyable(type: asdl.Sum | asdl.Product) -> bool:
        if isinstance(type, asdl.Sum):
            # A sum type is copyable if all of its alternatives are copyable
            return all(_helpers.copyable(alt) for alt in type.types)
        if isinstance(type, asdl.Product) or isinstance(type, asdl.Constructor):
            # A product type is copyable if it is fieldless
            return not type.fields
    
    @staticmethod
    def names_of_alts(sum_type: asdl.Sum) -> typing.List[str]:
        return [alt.name for alt in sum_type.types]

    @staticmethod
    def field_type(field: asdl.Field) -> str:
        raw_type: str = _helpers.type_name(field.type)
        
        if field.type in builtin_types:
            assert not field.opt, "Optional builtin types are not yet supported."
        
        if field.seq:
            return f"sequence<{raw_type}>"
        
        return "field<{}>{}".format(
            raw_type,
            " /* nullable */" if field.opt else "",
        )
    
    @staticmethod
    def field_decls(fields: typing.List[asdl.Field]) -> typing.List[str]:
        return [
            f"{_helpers.field_type(field)} {field.name}"
            for field in fields
        ]
    
    @staticmethod
    def type_name(type: str) -> str:
        return builtin_types.get(type, type)
    
    # @staticmethod
    # def all_abstract_names(module: asdl.Module) -> typing.List[str]:
    #     return [type.name for type in module.dfns]
    
    # @staticmethod
    # def all_concrete_names(module: asdl.Module) -> typing.List[str]:
    #     return [type.name for type in module.dfns if not isinstance(type.value, asdl.Sum)]


def main():
    args = parser.parse_args()
    
    asdl_file: pathlib.Path = args.asdl
    output_dir: pathlib.Path = args.output
    
    asdl_module: asdl.Module = asdl.parse(asdl_file)
    assert asdl.check(asdl_module, builtin_types)
    
    # TODO: Name the unnamed fields!
    
    env: jinja2.Environment = make_env()
    env.globals.update(dict(
        asdl_module=asdl_module,
        asdl=asdl,
        helpers=_helpers,
    ))
    
    render_tpl(
        env,
        "ast_nodes.tpl.hpp",
        output_dir / "include/bondrewd/ast/ast_nodes.generated.hpp",
    )


if __name__ == "__main__":
    exit(main())
