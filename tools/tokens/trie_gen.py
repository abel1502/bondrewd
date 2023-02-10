from __future__ import annotations
import typing
import dataclasses
from functools import cached_property


# class WordChecker:
#     _node: Node
#     _word: typing.List[str]
    
#     def __init__(self, node: Node):
#         self._node = node
#         self._word = []
    
#     def is_term(self) -> bool:
#         return self._node.is_term()
    
#     def go(self, ch: str) -> None:
#         assert len(ch) <= 1
        
#         if not ch:
#             return
        
#         self._node = self._node.get_child(ch)
#         self._word.append(ch)
    
#     def restart(self) -> None:
#         self._node = self._node.master._root
#         self._word = []
    
#     def get_verdict(self) -> typing.Any:
#         return self._node.verdict
    
#     def get_word(self) -> str:
#         return "".join(self._word)


@dataclasses.dataclass(eq=False)
class Node:
    verdict: typing.Any
    master: Trie
    parent: Node | None = None
    word: str = ""
    children: typing.Dict[str, Node] = dataclasses.field(default_factory=dict)
    # Fields below are autofilled
    index: int = None
    
    def __post_init__(self):
        if self.index is None:
            self.index = self.master._allocate_node_index()
    
    def is_term(self) -> bool:
        return not self.children
    
    def get_child(self, ch: str) -> Node:
        assert len(ch) == 1
        
        return self.children.get(ch, self.master._sentinel)
    
    def add_child(self, ch: str, verdict: typing.Any) -> Node:
        return self.children.setdefault(ch, Node(verdict, self.master, self, self.word + ch))
    
    @cached_property
    def rollback(self) -> Node:
        """
        Rollback is a (non-strict) parent node that has a positive verdict, if any.
        It is needed for the cases where a word is a prefix of another word.
        
        For example, if we have two puncts: `=` and `=!>`, and get `=!a` as input,
        when we see the `a`, we will use rollback to get the verdict for `=`
        instead of reporting an error.
        """
        
        if self.verdict is not self.master._verdict_none:
            return self
        
        if self.parent is None:
            return self.master._sentinel
        
        return self.parent.rollback


class Trie:
    _verdict_none: typing.Final[typing.Any]
    _next_node_index: int
    _root: typing.Final[Node]
    _sentinel: typing.Final[Node]
    _all_nodes: typing.Set[Node]
    
    def __init__(self, verdict_none: typing.Any = "none"):
        self._verdict_none = verdict_none
        self._next_node_index = 0
        self._root = Node(verdict_none, self)
        self._sentinel = Node(verdict_none, self)
        self._all_nodes = {self._root, self._sentinel}
    
    def add_word(self, word: str, verdict: typing.Any) -> None:
        verdict_none = self._verdict_none
        
        node = self._root
        for ch in word:
            node = node.add_child(ch, verdict_none)
            self._all_nodes.add(node)
        
        if node.verdict != verdict_none:
            raise ValueError(f"Word {word!r} already exists in the trie")
        
        node.verdict = verdict
    
    def add_wordlist(self, words: typing.Iterable[str], verdict: typing.Any) -> None:
        for word in words:
            self.add_word(word, verdict)
    
    # def create_checker(self) -> WordChecker:
    #     return WordChecker(self._root)
    
    def _allocate_node_index(self) -> int:
        index = self._next_node_index
        self._next_node_index += 1
        return index


@dataclasses.dataclass
class TrieInfo:
    verdicts: typing.List[str]
    root: Node
    
    @classmethod
    def from_trie(cls, trie: Trie) -> TrieInfo:
        verdicts: typing.Set[str] = set()
        
        for node in trie._all_nodes:
            verdict = node.verdict
            assert isinstance(verdict, str), f"All verdicts must be strings, not {verdict!r}"
            verdicts.add(verdict)
        
        assert trie._verdict_none in verdicts
        verdicts.remove(trie._verdict_none)
        
        verdicts = [trie._verdict_none] + list(sorted(verdicts))
        
        return cls(verdicts, trie._root)
