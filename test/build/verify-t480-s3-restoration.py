"""Verify the bounded S3 restorations against real compiler receipts.

Run after the before/after compiler captures, with build/t480-s3 as argument.
This does not equate arbitrary compiler differences or prove unrelated edits.
"""
import hashlib
import json
from pathlib import Path
import struct
import sys

ROOT = Path(__file__).resolve().parents[2]


def read(path):
    return json.loads(path.read_text(encoding='utf-8-sig'))


def coff(path, normalize_address=False):
    data = path.read_bytes()
    machine, count, _, symbol_offset, symbols, optional, flags = struct.unpack_from('<HHIIIHH', data)
    assert machine in (0x14c, 0x8664) and optional == 0
    sections = []
    for index in range(count):
        offset = 20 + index * 40
        name = data[offset:offset + 8].rstrip(b'\0')
        size, raw, reloc = struct.unpack_from('<III', data, offset + 16)
        reloc_count, line_count, attributes = struct.unpack_from('<HHI', data, offset + 32)
        assert line_count == 0
        body = data[raw:raw + size] if raw else b''
        # CodeView records include the different capture output paths. .chks64
        # carries compiler checksums, not loader-visible state or instructions.
        if name in (b'.debug$S', b'.chks64'):
            continue
        if normalize_address and index == 4:
            assert name == b'.text$mn' and body[1029:1033] == bytes.fromhex('88 44 3a 43')
            # The only changed instruction is mov [edx+edi+43h],al versus
            # mov [edi+edx+43h],al. Scale is one, neither base is ESP/EBP,
            # and both address forms use DS and identical 32-bit arithmetic.
            body = body[:1031] + b'\x17' + body[1032:]
        sections.append((index, name.hex(), size, attributes, body.hex(),
                         data[reloc:reloc + reloc_count * 10].hex() if reloc else ''))
    table = bytearray(data[symbol_offset:symbol_offset + symbols * 18])
    index = 0
    while index < symbols:
        offset = index * 18
        section, symbol_type, storage, auxiliary = struct.unpack_from('<hHBB', table, offset + 12)
        if storage == 3 and section > 0 and symbol_type == 0 and auxiliary == 1:
            # Section-definition checksum. All names, lengths, relocation
            # counts, selection modes and other symbol fields remain checked.
            table[offset + 18 + 8:offset + 18 + 12] = b'\0' * 4
        index += auxiliary + 1
    assert index == symbols
    return machine, flags, sections, table.hex(), data[symbol_offset + symbols * 18:].hex()


def main():
    work = Path(sys.argv[1]).resolve()
    assert work.is_relative_to(ROOT / 'build/t480-s3')
    before = read(work / 'objects-before/result.json')
    after = read(work / 'objects-after/result.json')
    expected = {'cmdexec', 'dpmiint', 'fpu'}
    assert {Path(r['source']).stem for r in before} == expected
    assert len(before) == len(after) == 3
    for old, new in zip(before, after):
        assert old['source'] == new['source'] and old['object'] == new['object']
        assert old['command'] == new['command'] and old['exit_code'] == new['exit_code'] == 0
        assert hashlib.sha256((ROOT / new['source']).read_bytes()).hexdigest() == new['source_sha256']
        name = Path(old['source']).stem
        assert coff(work / f'objects-before/{name}.obj') == coff(
            work / f'objects-after/{name}.obj', normalize_address=name == 'cmdexec'), name
    checked = 0
    for arch in ('x86', 'x64'):
        old = read(work / f'pp-token-before-{arch}/result.json')
        new = read(work / f'pp-token-after-{arch}/result.json')
        assert len(old) == len(new)
        for a, b in zip(old, new):
            for field in ('object', 'command_sha256', 'token_location_sha256', 'nonempty_lines', 'exit_code'):
                assert a[field] == b[field], (arch, field)
            assert b['exit_code'] == 0
        checked += len(new)
    assert checked == 13
    for row in read(work / 'spelling-restorations.json')['files']:
        assert hashlib.sha256((ROOT / row['path']).read_bytes()).hexdigest() == row['after_sha256']
    result = {'passed': True, 'actual_objects': 3, 'spelling_consumers': checked,
              'permitted_instruction_difference': 'one commutative scale-one x86 SIB address in cmdCheckBinary',
              'excluded_metadata': ['COFF timestamp', 'CodeView object path', 'compiler section checksums']}
    (work / 'restoration-test.json').write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
    print('PASS three actual COFF contracts and thirteen location-sensitive preprocessor consumers')


if __name__ == '__main__':
    main()
