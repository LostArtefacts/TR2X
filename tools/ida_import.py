#!/usr/bin/env python3
import ctypes
import re
import tempfile
from collections import defaultdict
from dataclasses import dataclass
from enum import StrEnum
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


class ProgressFileSection(StrEnum):
    TYPES = "types"
    FUNCTIONS = "functions"
    VARIABLES = "variables"


@dataclass
class Symbol:
    offset: int
    signature: str
    flags: str = ""


@dataclass
class ProgressFile:
    type_definitions: str
    functions: list[Symbol]
    variables: list[Symbol]


@dataclass
class ProgressFile:
    type_definitions: str
    functions: list[Symbol]
    variables: list[Symbol]


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
    result = ProgressFile(type_definitions="", functions=[], variables=[])

    section: ProgressFileSection | None = None
    for line in path.read_text(encoding="utf-8").splitlines():
        line = line.strip()
        if match := re.match("^# ([A-Z]+)$", line):
            section_name = match.group(1).lower()
            if section_name in list(ProgressFileSection):
                section = ProgressFileSection(section_name)

        if line.startswith("#") or not line:
            continue

        if section == ProgressFileSection.TYPES:
            result.type_definitions += line + "\n"

        if section == ProgressFileSection.FUNCTIONS:
            offset, size, flags, signature = re.split(r"\s+", line, maxsplit=3)
            result.functions.append(
                Symbol(
                    signature=signature,
                    offset=to_int(offset),
                    flags=flags,
                )
            )

        if section == ProgressFileSection.VARIABLES:
            offset, flags, signature = re.split(r"\s+", line, maxsplit=2)
            result.variables.append(
                Symbol(
                    signature=signature,
                    offset=to_int(offset),
                    flags=flags,
                )
            )

    return result


def import_types_from_file(path: Path) -> None:
    print(f"Importing type information:")
    if idaapi:
        error_count = idaapi.idc_parse_types(str(path), idc.PT_FILE)
    else:
        error_count = 0
    print(f"    done ({error_count} errors)")


def import_symbol(symbol: Symbol) -> None:
    known = not re.match(r"(\s+|^)(dword|sub)_", symbol.signature)

    if known:
        if idc:
            result = idc.parse_decl(symbol.signature, idc.PT_SILENT)
            if not result:
                raise RuntimeError("invalid signature", symbol.signature)

            name = result[0]
            if name.startswith("_"):
                name = name[1:]

            idc.set_name(symbol.offset, name)
            idc.apply_type(symbol.offset, result)

        if idaapi:
            til = idaapi.get_idati()
            ti = idaapi.tinfo_t()

            name = idaapi.parse_decl(ti, til, symbol.signature, idaapi.PT_VAR)
            if not name:
                raise RuntimeError("invalid signature", symbol.signature)

            if name.startswith("_"):
                name = name[1:]

            idaapi.set_name(symbol.offset, name)
            idaapi.apply_tinfo(symbol.offset, ti, 0)

    if idaapi:
        func_num = idaapi.get_func_num(symbol.offset)
        if func_num != -1:
            func_struct = idaapi.getn_func(func_num)
            if func_struct:
                # BGR
                if "+" in symbol.flags:
                    func_struct.color = 0xA0FFA0
                elif "x" in symbol.flags:
                    func_struct.color = 0xA0A0A0
                elif known:
                    func_struct.color = 0xA0FFFF
                else:
                    func_struct.color = 0xEEEEEE
                idaapi.update_func(func_struct)


def import_symbols(symbols: list[Symbol]) -> None:
    print(f"Importing symbols ({len(symbols)}):")
    error_count = 0
    for symbol in symbols:
        try:
            import_symbol(symbol)
        except Exception as ex:
            print("  error", ex)
            error_count += 1
    print(f"    done ({error_count} errors)")


def main():
    progress_file = parse_progress_file(PROGRESS_FILE)

    with tempfile.TemporaryDirectory() as tmpdir:
        tmpdir = Path(tmpdir)

        types_file = tmpdir / "types.txt"
        types_file.write_text(progress_file.type_definitions)

        import_types_from_file(types_file)
        import_symbols(progress_file.functions)
        import_symbols(progress_file.variables)


if __name__ == "__main__":
    main()
