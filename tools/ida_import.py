#!/usr/bin/env python3
import ctypes
import re
import tempfile
from collections import defaultdict
from dataclasses import dataclass
from enum import Enum
from pathlib import Path
from typing import Any, Dict, List, Optional

try:
    import idaapi
    import idc
except ImportError:
    idaapi = None
    idc = None

REPO_DIR = Path(__file__).parent.parent
DOCS_DIR = REPO_DIR / "docs"
PROGRESS_FILE = DOCS_DIR / "progress.txt"


class ProgressFileSection(Enum):
    TYPES = "types"
    FUNCTIONS = "functions"
    VARIABLES = "variables"


@dataclass
class Declaration:
    offset: int
    declaration: str
    flags: str = ""


@dataclass
class ProgressFile:
    type_definitions: str
    declarations: list[Declaration]


def to_int(source: str) -> Optional[int]:
    source = source.strip()
    if source.startswith("/*"):
        source = source[2:]
    if source.endswith("*/"):
        source = source[:-2]
    source = source.strip()
    if not source.replace("-", ""):
        return None
    if source.startswith(("0x", "0X")):
        source = source[2:]
    return int(source, 16)


def parse_progress_file(path: Path) -> ProgressFile:
    current_section: ProgressFileSection | None = None
    section_map: dict[ProgressFileSection.TYPES, list[str]] = defaultdict(list)
    for line in path.read_text().splitlines():
        for section in ProgressFileSection:
            if line == f"# {section.value.upper()}":
                current_section = section
        if line.startswith("#") or not line:
            continue
        if current_section is not None:
            section_map[current_section].append(line)

    declarations: list[Declaration] = []
    for line in section_map[ProgressFileSection.VARIABLES]:
        offset, declaration = re.split("\s+", line, maxsplit=1)
        offset = to_int(offset)
        declarations.append(
            Declaration(offset=offset, declaration=declaration)
        )

    for line in section_map[ProgressFileSection.FUNCTIONS]:
        offset, size, flags, declaration = re.split("\s+", line, maxsplit=3)
        offset = to_int(offset)
        declarations.append(
            Declaration(offset=offset, declaration=declaration, flags=flags)
        )

    return ProgressFile(
        type_definitions="\n".join(section_map[ProgressFileSection.TYPES]),
        declarations=declarations,
    )


def import_types_from_file(path: Path) -> None:
    print(f"Importing type information:")
    if idaapi:
        error_count = idaapi.idc_parse_types(str(path), idc.PT_FILE)
    else:
        error_count = 0
    print(f"    done ({error_count} errors)")


def import_declaration(declaration: Declaration) -> None:
    known = not re.match(r"(\s+|^)(dword|sub)_", declaration.declaration)

    if known:
        print(
            f"    renaming 0x{declaration.offset:08x} to {declaration.declaration}"
        )

        if idaapi:
            til = idaapi.get_idati()
            ti = idaapi.tinfo_t()

            name = idaapi.parse_decl(
                ti, til, declaration.declaration, idaapi.PT_VAR
            )
            if name.startswith("_"):
                name = name[1:]

            idaapi.set_name(declaration.offset, name)
            idaapi.apply_tinfo(declaration.offset, ti, 0)

    if idaapi:
        func_num = idaapi.get_func_num(declaration.offset)
        if func_num != -1:
            func_struct = idaapi.getn_func(func_num)
            if func_struct:
                # BGR
                if "+" in declaration.flags:
                    func_struct.color = 0xA0FFA0
                elif "x" in declaration.flags:
                    func_struct.color = 0xA0A0A0
                elif known:
                    func_struct.color = 0xA0FFFF
                else:
                    func_struct.color = 0xEEEEEE
                idaapi.update_func(func_struct)


def import_functions_from_file(declarations: list[Declaration]) -> None:
    print(f"Importing declarations:")
    for declaration in declarations:
        import_declaration(declaration)
    print("    done")


def main():
    progress_file = parse_progress_file(PROGRESS_FILE)

    with tempfile.TemporaryDirectory() as tmpdir:
        tmpdir = Path(tmpdir)

        types_file = tmpdir / "types.txt"
        types_file.write_text(progress_file.type_definitions)

        import_types_from_file(types_file)
        import_functions_from_file(progress_file.declarations)


if __name__ == "__main__":
    main()
