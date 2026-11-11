#!/usr/bin/env python3
"""Verify the NTVDM PE export name/ordinal contract without loading it."""
import argparse
import csv
import struct
from pathlib import Path


def read_exports(path):
    data = path.read_bytes()

    def u16(offset):
        return struct.unpack_from('<H', data, offset)[0]

    def u32(offset):
        return struct.unpack_from('<I', data, offset)[0]

    if data[:2] != b'MZ':
        raise ValueError('not an MZ image')
    pe = u32(0x3c)
    if data[pe:pe + 4] != b'PE\0\0':
        raise ValueError('not a PE image')
    sections = u16(pe + 6)
    optional = pe + 24
    magic = u16(optional)
    if magic not in (0x10b, 0x20b):
        raise ValueError('unknown PE optional header')
    directory = optional + (96 if magic == 0x10b else 112)
    export_rva = u32(directory)
    section_table = optional + u16(pe + 20)

    def offset(rva):
        for index in range(sections):
            section = section_table + index * 40
            virtual = u32(section + 12)
            size = u32(section + 16)  # only bytes actually present in the file
            if virtual <= rva < virtual + size:
                return u32(section + 20) + rva - virtual
        raise ValueError(f'RVA outside file-backed section: {rva:#x}')

    if not export_rva:
        raise ValueError('no export directory')
    directory = offset(export_rva)
    base = u32(directory + 16)
    functions = u32(directory + 20)
    count = u32(directory + 24)
    addresses = offset(u32(directory + 28))
    names = offset(u32(directory + 32))
    ordinals = offset(u32(directory + 36))
    result = {}
    for index in range(count):
        name_offset = offset(u32(names + index * 4))
        end = data.index(b'\0', name_offset)
        name = data[name_offset:end].decode('ascii')
        slot = u16(ordinals + index * 2)
        if slot >= functions or not u32(addresses + slot * 4) or name in result:
            raise ValueError(f'invalid or duplicate PE export: {name}')
        result[name] = base + slot
    return result


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('image', type=Path)
    parser.add_argument('contract', type=Path)
    args = parser.parse_args()
    with args.contract.open(encoding='utf-8-sig', newline='') as stream:
        rows = list(csv.DictReader(stream))
    expected = {}
    for row in rows:
        name, ordinal = row['name'], int(row['ordinal'])
        if not name or not 1 <= ordinal <= 65535 or name in expected or ordinal in expected.values():
            raise ValueError(f'invalid or duplicate contract: {name} {ordinal}')
        expected[name] = ordinal
    if not expected:
        raise ValueError('empty export contract')
    actual = read_exports(args.image)
    errors = [(name, ordinal, actual.get(name)) for name, ordinal in expected.items()
              if actual.get(name) != ordinal]
    if errors:
        for name, wanted, found in errors:
            print(f'FAIL {name}: expected {wanted}, got {found}')
        return 1
    print(f'PASS NTVDM PE ordinals: {len(expected)} contract entries, {len(actual)} named exports')
    return 0


if __name__ == '__main__':
    try:
        raise SystemExit(main())
    except (OSError, ValueError, KeyError, struct.error) as error:
        raise SystemExit(f'FAIL NTVDM PE export check: {error}')
