"""Compare link-relevant COFF sections and relocations, ignoring debug data."""
import argparse
import hashlib
from pathlib import Path
import struct


def object_sections(path):
    data = path.read_bytes()
    if len(data) < 20:
        raise ValueError(f"truncated COFF header: {path}")
    machine, count, _, symbol_offset, symbol_count, optional, _ = struct.unpack_from("<HHIIIHH", data)
    if machine not in (0x14C, 0x8664) or optional:
        raise ValueError(f"unsupported COFF format: {path}")
    string_offset = symbol_offset + symbol_count * 18
    if string_offset + 4 > len(data):
        raise ValueError(f"missing COFF string table: {path}")
    string_size = struct.unpack_from("<I", data, string_offset)[0]
    if string_size < 4 or string_offset + string_size > len(data):
        raise ValueError(f"invalid COFF string table: {path}")

    def string_at(offset):
        if offset < 4 or offset >= string_size:
            raise ValueError(f"invalid COFF string offset: {path}")
        start = string_offset + offset
        end = data.find(b"\0", start, string_offset + string_size)
        if end < 0:
            raise ValueError(f"unterminated COFF string: {path}")
        return data[start:end]

    def name_at(raw):
        if raw[:4] == b"\0\0\0\0":
            return string_at(struct.unpack_from("<I", raw, 4)[0])
        short = raw.rstrip(b"\0")
        if short.startswith(b"/") and short[1:].isdigit():
            return string_at(int(short[1:]))
        return short

    symbols = {}
    index = 0
    while index < symbol_count:
        offset = symbol_offset + index * 18
        if offset + 18 > len(data):
            raise ValueError(f"truncated COFF symbols: {path}")
        name = name_at(data[offset:offset + 8])
        value, section, kind, storage, auxiliaries = struct.unpack_from("<IhHBB", data, offset + 8)
        symbols[index] = (name, value, section, kind, storage)
        index += 1 + auxiliaries
    sections = []
    for index in range(count):
        offset = 20 + index * 40
        if offset + 40 > len(data):
            raise ValueError(f"truncated COFF section table: {path}")
        name = name_at(data[offset:offset + 8])
        virtual_size, _, size, raw, reloc, _, reloc_count, _, flags = struct.unpack_from(
            "<IIIIIIHHI", data, offset + 8)
        if name.startswith(b".debug") or name == b".chks64":
            continue
        if flags & 0x01000000:
            raise ValueError(f"extended COFF relocations require review: {path}")
        uninitialized = bool(flags & 0x00000080)
        if (size and ((raw == 0 and not uninitialized) or (raw and raw + size > len(data)))) or reloc + reloc_count * 10 > len(data):
            raise ValueError(f"invalid COFF section bounds: {path}")
        relocations = []
        for rel_index in range(reloc_count):
            address, symbol, kind = struct.unpack_from("<IIH", data, reloc + rel_index * 10)
            if symbol not in symbols:
                raise ValueError(f"unsupported COFF relocation symbol: {path}")
            relocations.append((address, symbols[symbol], kind))
        sections.append((name, virtual_size, size, flags, data[raw:raw + size] if raw and size else b"",
                         tuple(relocations)))
    return machine, sections


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("baseline", type=Path)
    parser.add_argument("shadow", type=Path)
    args = parser.parse_args()
    baseline = object_sections(args.baseline)
    shadow = object_sections(args.shadow)
    if baseline != shadow:
        raise SystemExit("link-relevant COFF sections or relocations differ")
    print(f"PASS {len(baseline[1])} link-relevant COFF sections and relocations; "
          f"baseline={hashlib.sha256(args.baseline.read_bytes()).hexdigest()} "
          f"shadow={hashlib.sha256(args.shadow.read_bytes()).hexdigest()}")


if __name__ == "__main__":
    main()
