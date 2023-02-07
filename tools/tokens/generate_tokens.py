from __future__ import annotations
import typing
import argparse
import pathlib
import textwrap
import jinja2
import datetime
import dataclasses
import codecs


from trie_gen import Trie, TrieInfo


LINE_COMMENTS: typing.Final[typing.Collection[str]] = ("//", "#")
BLOCK_COMMENT: typing.Final[typing.Tuple[str, str]] = ("/*", "*/")
STRING_QUOTES: typing.Final[typing.Collection[str]] = ("'", '"')
STRING_ESCAPES: typing.Final[typing.Collection[str]] = ("\\", "\n", "'", '"', "0", "n", "r", "t")


parser = argparse.ArgumentParser(
    description=textwrap.dedent("""
    The script responsible for generating several token-related source files.
    
    More precisely, the script generates the following files:
     - include/bondrewd/lex/tokens.generated.hpp
     - src/lex/tokens.generated.cpp
    """)
)

parser.add_argument(
    "-k", "--keywords",
    type=pathlib.Path,
    help="The path to the keywords listing file.",
    default=pathlib.Path(__file__).parent.parent.parent / "grammar/keywords",
)

parser.add_argument(
    "-p", "--puncts",
    type=pathlib.Path,
    help="The path to the punctuations listing file.",
    default=pathlib.Path(__file__).parent.parent.parent / "grammar/puncts",
)

parser.add_argument(
    "-o", "--output",
    type=pathlib.Path,
    help="The path to the root of the interpreter directory.",
    default=pathlib.Path(__file__).parent.parent.parent,
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
        return [ListingItem.from_line(line) for line in f]


@dataclasses.dataclass
class TokensInfo:
    keywords: Listing
    puncts: Listing
    punct_lookup: typing.Dict[str, str]
    misc_trie: TrieInfo
    string_trie: TrieInfo
    block_comment_trie: TrieInfo
    
    @classmethod
    def process(cls, keywords: Listing, puncts: Listing) -> TokensInfo:
        punct_lookup = {punct.value: punct.name for punct in puncts}
        
        misc_trie = Trie()
        string_trie = Trie()
        block_comment_trie = Trie()
        
        misc_trie.add_wordlist(LINE_COMMENTS, "line_comment")
        misc_trie.add_word(BLOCK_COMMENT[0], "block_comment")
        misc_trie.add_wordlist(punct_lookup.keys(), "punct")
        misc_trie.add_wordlist(STRING_QUOTES, "string_quote")
        
        string_trie.add_wordlist(("\\" + k for k in STRING_ESCAPES), "escape")
        string_trie.add_wordlist(STRING_QUOTES, "end_quote")
        
        block_comment_trie.add_word(BLOCK_COMMENT[0], "start")
        block_comment_trie.add_word(BLOCK_COMMENT[1], "end")

        return cls(
            keywords,
            puncts,
            punct_lookup,
            TrieInfo.from_trie(misc_trie),
            TrieInfo.from_trie(string_trie),
            TrieInfo.from_trie(block_comment_trie),
        )


def render_templates(output_dir: pathlib.Path, tokens_info: TokensInfo):
    env = jinja2.Environment(
        loader=jinja2.FileSystemLoader(pathlib.Path(__file__).parent / "templates"),
        autoescape=False,
    )
    
    env.filters["cpp_escape"] = lambda s: codecs.encode(s, "unicode_escape").decode("ascii").replace("'", "\\'")
    
    env.get_template("tokens.tpl.hpp").stream(
        date=datetime.datetime.utcnow().strftime("%Y-%m-%d %H:%M:%S UTC"),
        tokens_info=tokens_info,
    ).dump(str(output_dir / "include/bondrewd/lex/tokens.generated.hpp"))
    
    env.get_template("tokens.tpl.cpp").stream(
        date=datetime.datetime.utcnow().strftime("%Y-%m-%d %H:%M:%S UTC"),
        tokens_info=tokens_info,
    ).dump(str(output_dir / "src/lex/tokens.generated.cpp"))


def main():
    args = parser.parse_args()
    
    keywords_file: pathlib.Path = args.keywords
    puncts_file: pathlib.Path = args.puncts
    output_dir: pathlib.Path = args.output
    
    keywords: typing.List[typing.Tuple[str, str]] = read_listing(keywords_file)
    puncts: typing.List[typing.Tuple[str, str]] = read_listing(puncts_file)
    
    tokens_info = TokensInfo.process(keywords, puncts)
    
    render_templates(output_dir, tokens_info)


if __name__ == "__main__":
    exit(main())
