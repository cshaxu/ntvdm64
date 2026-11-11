#!/usr/bin/env python3
"""Verify both reports of the isolated Win3.1/DOS allocation witness."""
import argparse
from pathlib import Path
import struct
import sys


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('parent', type=Path)
    parser.add_argument('child', type=Path)
    args = parser.parse_args()
    try:
        parent = args.parent.read_bytes()
        child = args.child.read_bytes()
        record = struct.Struct('<4sHHHHHHH')
        if len(parent) != 6 * record.size:
            raise ValueError('parent must contain all six checkpoints')
        rows = [record.unpack_from(parent, i * record.size) for i in range(6)]
        if any(row[0] != b'M31A' or row[1] != i + 1 for i, row in enumerate(rows)):
            raise ValueError('parent checkpoint signature or order is wrong')
        handle, segment = rows[1][2:4]
        if not handle or not segment or segment == 0xffff:
            raise ValueError('parent did not allocate ordinary DOS memory')
        if any(row[2:4] != (handle, segment) for row in rows[1:]):
            raise ValueError('allocation identity changed across checkpoints')
        if rows[2][5] != 0x5aa5 or rows[3][6] < 32 or rows[4][7] != 1:
            raise ValueError('parent write, child launch, or child completion failed')
        if rows[5][4] != 0:
            raise ValueError('parent allocation was not freed')
        if len(child) != 8:
            raise ValueError('child report has the wrong size')
        magic, observed_segment, observed_value = struct.unpack('<4sHH', child)
        if (magic, observed_segment, observed_value) != (b'M31V', segment, 0x5aa5):
            raise ValueError('child did not observe the parent marker at the requested segment')
    except (OSError, ValueError, struct.error) as exc:
        print(f'FAIL: {exc}', file=sys.stderr)
        return 1
    print('PASS: parent allocation, DOS child visibility, and release')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
