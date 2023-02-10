from __future__ import annotations
import typing
import pathlib
import jinja2
import datetime
import codecs
import inspect


_AUTOGENERATED_LABEL: typing.Final[str] = """\
// AUTOGENERATED by {tool} on {date}
// DO NOT EDIT
"""


PROJECT_ROOT: typing.Final[pathlib.Path] = pathlib.Path(__file__).parent.parent.parent
TOOLS_ROOT: typing.Final[pathlib.Path] = PROJECT_ROOT / "tools"


def _get_tool_path() -> pathlib.Path | None:
    for frame_info in inspect.stack():
        if frame_info.function == "<module>" \
            and pathlib.Path(frame_info.filename) != pathlib.Path(__file__):
            
            filename: str = frame_info.filename
            break
    else:
        assert False, "Could not find the tool file name."
    
    if filename == "<stdin>":
        return None
    
    return pathlib.Path(filename)


def _get_tool_proj_path() -> pathlib.Path | None:
    tool_path = _get_tool_path()
    if tool_path is None:
        return None
    
    return tool_path.relative_to(PROJECT_ROOT)


def _get_autogen_label(tool_name: str | None = None) -> str:
    if tool_name is None:
        tool_name = str(_get_tool_proj_path())
    
    return _AUTOGENERATED_LABEL.format(
        tool=tool_name,
        date=datetime.datetime.utcnow().isoformat(sep=" ", timespec="seconds"),
    )


def make_env(template_dir: pathlib.Path | str | None = None) -> jinja2.Environment:
    if template_dir is None:
        template_dir = _get_tool_path().parent / "templates"
    
    env = jinja2.Environment(
        loader=jinja2.FileSystemLoader(template_dir),
        autoescape=False,
    )
    
    env.filters["cpp_escape"] = lambda s: codecs.encode(s, "unicode_escape").decode("ascii")\
        .replace("'", "\\'").replace('"', '\\"')
    
    env.globals["_autogenerated_"] = _get_autogen_label()
    
    return env


def render_tpl(env: jinja2.Environment, tpl_name: str, output_file: pathlib.Path | str, **kwargs) -> None:
    env.get_template(tpl_name).stream(**kwargs).dump(str(output_file))


__all__ = [
    "jinja2",
    "PROJECT_ROOT",
    "TOOLS_ROOT",
    "make_env",
    "render_tpl",
]