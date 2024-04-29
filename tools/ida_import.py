#!/usr/bin/env python3
import ctypes
import re
import tempfile
from collections import defaultdict
from dataclasses import dataclass
from enum import StrEnum
from pathlib import Path
from typing import Any, Dict, List, Optional

from tr2x.ida_progress import parse_progress_file, Symbol
from tr2x.paths import TR2X_PROGRESS_FILE

try:
    import idaapi
    import idc
except ImportError:
    idaapi = None
    idc = None




def import_types(types: list[str]) -> None:
    print(f"Importing types ({len(types)}):")
    for definition in types:
        if idaapi:
            error_count = idaapi.idc_parse_types(definition, 0)
        else:
            error_count = 0
        if error_count > 0:
            print('Error:', definition)
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
    progress_file = parse_progress_file(TR2X_PROGRESS_FILE)

    with tempfile.TemporaryDirectory() as tmpdir:
        tmpdir = Path(tmpdir)

        import_types(progress_file.types)
        import_symbols(progress_file.functions)
        import_symbols(progress_file.variables)


if __name__ == "__main__":
    main()
