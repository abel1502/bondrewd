from __future__ import annotations
import typing
import pathlib
import dataclasses
from ast import literal_eval


@dataclasses.dataclass
class ListingItem:
    name: str
    quoted_value: str
    
    @classmethod
    def from_line(cls, line: str) -> ListingItem:
        name, quoted_value = line.strip().split(maxsplit=1)
        return cls(name, quoted_value)
    
    @property
    def str_value(self) -> str:
        return literal_eval(self.quoted_value)


Listing = typing.List[ListingItem]


def read_listing(path: pathlib.Path) -> Listing:
    with path.open("r") as f:
        lines = f.readlines()
    
    lines = (line.strip() for line in lines)
    lines = (line for line in lines if line and not line.startswith("#"))
    
    return [ListingItem.from_line(line) for line in lines]


__all__ = [
    "ListingItem",
    "Listing",
    "read_listing",
]
