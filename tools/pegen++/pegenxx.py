from __future__ import annotations
import typing
import argparse
import pathlib
import dataclasses

import sys
sys.path.append(str(pathlib.Path(__file__).parent.parent))
from jinja_codegen import *
from token_listing import *

from cxx_generator import CXXParserGenerator
from pegen.grammar import Grammar

from custom_pegen_grammar_parser import parse_grammar


parser = argparse.ArgumentParser("""\
The script responsible for generating several parser-related source files.

More precisely, the script generates the following files:
    - include/bondrewd/parse/parser.gen.hpp
    - src/parse/parser.gen.cpp
""")

parser.add_argument(
    "-k", "--keywords",
    type=pathlib.Path,
    help="The path to the keywords listing file.",
    default=PROJECT_ROOT / "grammar/keywords",
)

parser.add_argument(
    "-p", "--puncts",
    type=pathlib.Path,
    help="The path to the punctuations listing file.",
    default=PROJECT_ROOT / "grammar/puncts",
)

parser.add_argument(
    "-g", "--grammar",
    type=pathlib.Path,
    help="The path to the grammar definition file.",
    default=PROJECT_ROOT / "grammar/bondrewd.gram",
)

parser.add_argument(
    "-o", "--output",
    type=pathlib.Path,
    help="The path to the root of the interpreter directory.",
    default=PROJECT_ROOT,
)


def main():
    args = parser.parse_args()
    
    keywords_file: pathlib.Path = args.keywords
    puncts_file: pathlib.Path = args.puncts
    grammar_file: pathlib.Path = args.grammar
    output_dir: pathlib.Path = args.output
    
    keywords: Listing = read_listing(keywords_file)
    puncts: Listing = read_listing(puncts_file)
    
    grammar: Grammar = parse_grammar(grammar_file)
    
    tokens_to_names: typing.Dict[str, str] = {
        k.str_value: k.name for k in keywords
    } | {
        p.str_value: p.name for p in puncts
    }
    
    generator = CXXParserGenerator(
        grammar,
        tokens_to_names,
    ).prepare()
    
    env = make_env()
    
    render_tpl(
        env,
        "parser.tpl.hpp",
        output_dir / "include/bondrewd/parse/parser.gen.hpp",
        generator=generator,
    )
    
    render_tpl(
        env,
        "parser.tpl.cpp",
        output_dir / "src/parse/parser.gen.cpp",
        generator=generator,
    )


if __name__ == "__main__":
    exit(main())
