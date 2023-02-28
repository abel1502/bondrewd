from __future__ import annotations
import typing
import pathlib
import dataclasses


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


__all__ = [
    "ListingItem",
    "Listing",
    "read_listing",
]
