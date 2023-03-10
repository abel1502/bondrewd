from __future__ import annotations
import typing
import pathlib
# import re
from ast import literal_eval

import sys
PROJECT_ROOT = pathlib.Path(__file__).parent.parent.parent
sys.path.append(str(PROJECT_ROOT / "tools"))
from token_listing import ListingItem, Listing, read_listing

from pygments import token, lexer
from sphinx.highlighting import lexers as sphinx_lexers


keywords: Listing = read_listing(PROJECT_ROOT / "grammar/keywords")
puncts: Listing = read_listing(PROJECT_ROOT / "grammar/puncts")


# TODO: account for the more complex stuff!
class BondrewdLexer(lexer.RegexLexer):
    name = 'bondrewd'

    tokens = {
        'root': [
            (lexer.words([kw.str_value for kw in keywords], prefix=r'\b', suffix=r'\b'), token.Keyword),
            (r'(?://|#).*?$', token.Comment.Singleline),
            (r'/\*', token.Comment.Multiline, 'comment'),
            (r'[a-zA-Z_][a-zA-Z_\d]*', token.Name),
            (r'0x', token.Number.Hex, 'number_hex'),
            (r'0o', token.Number.Oct, 'number_oct'),
            (r'0b', token.Number.Bin, 'number_bin'),
            (r'(?=\d|\.\d)', token.Number, 'number_dec'),
            (r'"""', token.String, 'string_md'),
            (r'"', token.String, 'string_d'),
            (r'\'\'\'', token.String, 'string_ms'),
            (r'\'', token.String, 'string_s'),
            (lexer.words([p.str_value for p in puncts]), token.Punctuation),
            (r'\s', token.Text),
        ],
        'comment': [
            (r'[^\*/]+', token.Comment.Multiline),
            (r'/\*', token.Comment.Multiline, '#push'),
            (r'\*/', token.Comment.Multiline, '#pop'),
            (r'[\*/]', token.Comment.Multiline)
        ],
        'string_ms': [
            (r'\\.', token.String.Escape),
            (r'\'\'\'', token.String, '#pop'),
            (r'\'', token.String),
            (r'[^\\\']+', token.String),
        ],
        'string_s': [
            (r'\\.', token.String.Escape),
            (r'[\'\n]', token.String, '#pop'),
            (r'[^\\\'\n]+', token.String),
        ],
        'string_md': [
            (r'\\.', token.String.Escape),
            (r'"""', token.String, '#pop'),
            (r'"', token.String),
            (r'[^\\"]+', token.String),
        ],
        'string_d': [
            (r'\\.', token.String.Escape),
            (r'["\n]', token.String, '#pop'),
            (r'[^\\"\n]+', token.String),
        ],
        'number_hex': [
            (r'[0-9a-fA-F]*(?:.[0-9a-fA-F]*)?(?:[pP][0-9a-fA-F]*)?', token.Number.Hex, '#pop'),
        ],
        'number_dec': [
            (r'[0-9]*(?:.[0-9]*)?(?:[eE][0-9]*)?', token.Number.Dec, '#pop'),
        ],
        'number_oct': [
            (r'[0-7]*(?:.[0-7]*)?(?:[eE][0-7]*)?', token.Number.Oct, '#pop'),
        ],
        'number_bin': [
            (r'[01]*(?:.[01]*)?(?:[eE][01]*)?', token.Number.Bin, '#pop'),
        ],
    }

sphinx_lexers['bondrewd'] = BondrewdLexer(startinline=True)