from __future__ import annotations
import typing
import argparse
import pathlib
import textwrap
import dataclasses
from ast import literal_eval

import sys
sys.path.append(str(pathlib.Path(__file__).parent.parent))
from jinja_codegen import *

from trie_gen import Trie, TrieInfo


LINE_COMMENTS: typing.Final[typing.Collection[str]] = ("//", "#")
BLOCK_COMMENT: typing.Final[typing.Tuple[str, str]] = ("/*", "*/")
STRING_QUOTES: typing.Final[typing.Collection[str]] = ("'", '"')


parser = argparse.ArgumentParser(
    description=textwrap.dedent("""
    The script responsible for generating several token-related source files.
    
    More precisely, the script generates the following files:
     - include/bondrewd/lex/tokens.gen.hpp
     - src/lex/tokens.gen.cpp
    """)
)

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
    "-o", "--output",
    type=pathlib.Path,
    help="The path to the root of the interpreter directory.",
    default=PROJECT_ROOT,
)


@dataclasses.dataclass
class ListingItem:
    name: str
    value: str
    
    @classmethod
    def from_line(cls, line: str) -> ListingItem:
        name, value = line.strip().split(maxsplit=1)
        return cls(name, value)


Listing = typing.List[ListingItem]


def read_listing(path: pathlib.Path) -> Listing:
    with path.open("r") as f:
        lines = f.readlines()
    
    lines = (line.strip() for line in lines)
    lines = (line for line in lines if line and not line.startswith("#"))
    
    return [ListingItem.from_line(line) for line in lines]


@dataclasses.dataclass
class TokensInfo:
    keywords: Listing
    puncts: Listing
    punct_lookup_quoted: typing.Dict[str, str]
    punct_lookup: typing.Dict[str, str]
    misc_trie: TrieInfo
    string_trie: TrieInfo
    block_comment_trie: TrieInfo
    
    @classmethod
    def process(cls, keywords: Listing, puncts: Listing) -> TokensInfo:
        punct_lookup_quoted = {punct.value: punct.name for punct in puncts}
        punct_lookup = {literal_eval(key): value for key, value in punct_lookup_quoted.items()}
        
        misc_trie = Trie()
        string_trie = Trie()
        block_comment_trie = Trie()
        
        misc_trie.add_wordlist(LINE_COMMENTS, "line_comment")
        misc_trie.add_word(BLOCK_COMMENT[0], "block_comment")
        misc_trie.add_wordlist(punct_lookup.keys(), "punct")
        misc_trie.add_wordlist(STRING_QUOTES, "string_quote")
        
        string_trie.add_word("\\", "escape")  # A bit of a hack, actually
        string_trie.add_wordlist(STRING_QUOTES, "end_quote")
        
        block_comment_trie.add_word(BLOCK_COMMENT[0], "start")
        block_comment_trie.add_word(BLOCK_COMMENT[1], "end")

        return cls(
            keywords,
            puncts,
            punct_lookup_quoted,
            punct_lookup,
            TrieInfo.from_trie(misc_trie),
            TrieInfo.from_trie(string_trie),
            TrieInfo.from_trie(block_comment_trie),
        )


def main():
    args = parser.parse_args()
    
    keywords_file: pathlib.Path = args.keywords
    puncts_file: pathlib.Path = args.puncts
    output_dir: pathlib.Path = args.output
    
    keywords: typing.List[typing.Tuple[str, str]] = read_listing(keywords_file)
    puncts: typing.List[typing.Tuple[str, str]] = read_listing(puncts_file)
    
    tokens_info = TokensInfo.process(keywords, puncts)
    
    env: jinja2.Environment = make_env()
    
    render_tpl(
        env,
        "tokens.tpl.hpp",
        output_dir / "include/bondrewd/lex/tokens.gen.hpp",
        tokens_info=tokens_info
    )
    
    render_tpl(
        env,
        "tokens.tpl.cpp",
        output_dir / "src/lex/tokens.gen.cpp",
        tokens_info=tokens_info
    )


if __name__ == "__main__":
    exit(main())
