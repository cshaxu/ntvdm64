"""Check the actual Win31 bridge/USER callback witness, not desktop input."""
import argparse
import json
from pathlib import Path
import struct

p = argparse.ArgumentParser()
p.add_argument('report', type=Path)
p.add_argument('--mode', required=True, choices=['standard', 'enhanced'])
a = p.parse_args()
raw = a.report.read_bytes()
record = struct.Struct('<4s16H')
assert raw and len(raw) % record.size == 0, 'Missing or incomplete guest records'
rows = [record.unpack_from(raw, i) for i in range(0, len(raw), record.size)]
assert all(r[0] == b'M31W' and r[2] == 0 for r in rows), 'Guest error or wrong record'
assert [r[1] for r in rows] == list(range(1, 8)), 'Not all callback phases completed'
r = rows[-1]
assert r[3] == 0x0a03, 'Not actual Windows 3.10'
flag = 0x10 if a.mode == 'standard' else 0x20
assert r[4] & flag, 'Windows is running a different mode'
assert r[5] and r[7], 'No loaded driver data selector or DPMI callback segment'
assert r[8] == 6, 'Not all six actual DPMI calls returned'
assert 99 <= r[9] <= 100 and 79 <= r[10] <= 80
assert 519 <= r[11] <= 520 and 399 <= r[12] <= 400
assert r[13] & 0x8000 and not r[14] & 0x8000, 'Left button down/up failed'
assert r[15] & 0x8000 and not r[16] & 0x8000, 'Right button down/up failed'
print(json.dumps(dict(passed=True, mode=a.mode, version=hex(r[3]), winflags=hex(r[4]),
                      events=r[8], first=list(r[9:11]), second=list(r[11:13]),
                      left=list(r[13:15]), right=list(r[15:17]),
                      scope='Real Win31 loaded driver -> DPMI -> USER cursor/button state; no physical input or UI interaction acceptance'), indent=2))
