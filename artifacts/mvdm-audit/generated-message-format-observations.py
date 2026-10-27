"""Bounded format observations for the frozen S10 message/artifact cohort.

SYM layout reference only: OpenNT sdktools/sprof/sym16.h and sym16.c.
That original header calls its layout empirical; this is not a full symbol
resolver or source-to-image proof. No historical executable is run.
"""
import csv
import hashlib
import io
import json
import re
import struct
import subprocess
import sys
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = '4837d3893'


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest().upper()


def inspect_sym(data):
    assert len(data) >= 16
    name_length = data[15]
    assert 0 < name_length <= 255 and 16 + name_length <= len(data)
    name = data[16:16 + name_length].decode('ascii')
    assert all(32 <= ord(c) < 127 for c in name)
    first = struct.unpack_from('<H', data, 12)[0] * 16
    assert first >= 16 + name_length and first + 21 <= len(data)
    segments, seen, position = [], set(), first
    while position:
        if position in seen:
            assert position == first
            break
        assert position + 21 <= len(data) and len(seen) < 4096
        seen.add(position)
        following, count, unknown, number = struct.unpack_from('<HHHH', data, position)
        length = data[position + 20]
        cursor = position + 21 + length
        assert length > 0 and cursor <= len(data)
        segment_name = data[position + 21:cursor].decode('ascii')
        assert all(32 <= ord(c) < 127 for c in segment_name)
        for _ in range(count):
            assert cursor + 3 <= len(data)
            symbol_length = data[cursor + 2]
            assert symbol_length > 0 and cursor + 3 + symbol_length <= len(data)
            symbol = data[cursor + 3:cursor + 3 + symbol_length]
            assert all(32 <= c < 127 for c in symbol)
            cursor += 3 + symbol_length
        segments.append(dict(offset=position, name=segment_name, number=number,
                             symbols=count, parsed_end=cursor, next=following * 16,
                             uninterpreted_word=unknown))
        position = following * 16
    assert segments
    return dict(module=name, segments=segments, symbols=sum(r['symbols'] for r in segments),
                limit='Empirical 16-bit segment/symbol framing only; constants, indexes, trailing records and source-to-image lineage unproved')


def inspect_omf(data):
    position, types, zeros, bad = 0, [], 0, []
    while position < len(data):
        assert position + 3 <= len(data)
        size = struct.unpack_from('<H', data, position + 1)[0]
        assert size > 0 and position + 3 + size <= len(data)
        record = data[position:position + 3 + size]
        types.append(record[0])
        if record[-1] == 0:
            zeros += 1
        elif sum(record) & 255:
            bad.append(position)
        position += 3 + size
    assert types[0] in (0x80, 0x82) and types[-1] in (0x8a, 0x8b)
    return dict(records=len(types), stored_zero_checksums=zeros,
                inconsistent_nonzero_checksums=bad,
                limit='OMF record framing is not link membership, relocation validity or runtime behavior')


def main(output):
    old = subprocess.check_output(['git', 'show', f'{BASE}:{A.as_posix()}/combined-initial-status.csv'])
    pending = [r['relative_path'] for r in csv.DictReader(io.StringIO(old.decode('utf-8-sig')))
               if r['status'] == 'pending' and re.search(r'\.(CL[0-9A-Z]|CTL|pch|sym|OBJ|tmp)$', r['relative_path'], re.I)]
    assert len(pending) == 76
    reference_paths = [Path('O:/repos.external/OpenNT/sdktools/sprof') / name for name in ('sym16.h', 'sym16.c')]
    records = []
    for relative in pending:
        path = Path('src/mvdm') / relative
        data = path.read_bytes()
        row = dict(relative_path=relative, sha256=sha(path), bytes=len(data))
        extension = path.suffix.lower()
        try:
            if extension == '.sym':
                row.update(format='empirical-16-bit-SYM', details=inspect_sym(data))
            elif extension == '.obj':
                row.update(format='OMF', details=inspect_omf(data))
            elif extension.startswith('.cl') or extension == '.ctl':
                assert b'\0' not in data
                text = data.decode('latin1')
                assert re.search(r'\b(?:DB|DW|EQU|LABEL|PUBLIC|INCLUDE)\b', text, re.I)
                row.update(format='MASM-message-or-control-text', lines=len(text.splitlines()),
                           limit='Text syntax observation; exact producer/consumer and conditional target require original source trace')
            elif extension == '.pch':
                assert data.startswith(b'VCPCH0')
                row.update(format='VCPCH0-prefixed-historical-PCH', limit='Observed signature only, not a complete PCH parser or compatible compiler claim')
            elif relative == 'wow16/write/insert.tmp':
                entries = [line for line in data.decode('latin1').splitlines() if line.strip('\x1a\x00 ')]
                source = path.with_suffix('.c').read_text(encoding='latin1').splitlines()
                matches = 0
                for entry in entries:
                    match = re.fullmatch(r'(\d+):(#ifdef DBCS)', entry)
                    assert match
                    number = int(match.group(1))
                    matches += number <= len(source) and source[number - 1].strip() == match.group(2)
                row.update(format='source-line-index-text', entries=len(entries), current_source_line_matches=matches,
                           compared_source=str(path.with_suffix('.c')), compared_source_sha256=sha(path.with_suffix('.c')),
                           limit='Line-index syntax only; mismatched positions disprove exact current-source generation, not the historical index-text role')
            else:
                row.update(format='retained-tmp-needs-owner-trace', prefix_hex=data[:32].hex())
            row['observation'] = 'pass'
        except (AssertionError, UnicodeDecodeError, struct.error) as error:
            row.update(observation='unresolved-format', reason=type(error).__name__)
        records.append(row)
    result = dict(result='observed', base=BASE, paths=len(records),
                  script_sha256=sha(Path(__file__)),
                  references=[dict(path=str(p), sha256=sha(p)) for p in reference_paths],
                  records=records,
                  scope='Read-only format observations, not capability classifications or runtime acceptance')
    output.write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(dict(paths=len(records), unresolved=[r['relative_path'] for r in records if r['observation'] != 'pass'])))


if __name__ == '__main__':
    main(Path(sys.argv[1]))
