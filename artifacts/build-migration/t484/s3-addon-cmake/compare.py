import hashlib
import json
import struct
from pathlib import Path

root = Path(__file__).resolve().parent
tree = root.parent / '.ninja/addon'

def timestamp_offsets(data):
    pe = struct.unpack_from('<I', data, 0x3c)[0]
    assert data[pe:pe + 4] == b'PE\0\0'
    optional = pe + 24
    assert struct.unpack_from('<H', data, optional)[0] == 0x20b
    rva, size = struct.unpack_from('<II', data, optional + 112 + 6 * 8)
    sections = pe + 24 + struct.unpack_from('<H', data, pe + 20)[0]
    offsets = set(range(pe + 8, pe + 12))
    for i in range(struct.unpack_from('<H', data, pe + 6)[0]):
        virtual_size, virtual, raw_size, raw = struct.unpack_from(
            '<IIII', data, sections + i * 40 + 8)
        if virtual <= rva and rva + size <= virtual + max(virtual_size, raw_size):
            assert size % 28 == 0
            for entry in range(raw + rva - virtual, raw + rva - virtual + size, 28):
                offsets.update(range(entry + 4, entry + 8))
            return offsets
    assert not size, 'Unmapped PE debug directory'
    return offsets

rows = []
for old in sorted((root / 'before').iterdir()):
    matches = [p for p in tree.rglob('*') if p.is_file()
               and p.name.lower() == old.name.lower()
               and p.parent.name != 'compat95-control']
    assert len(matches) == 1, (old.name, matches)
    new = matches[0]
    a, b = old.read_bytes(), new.read_bytes()
    assert len(a) == len(b), old.name
    changes = [i for i, (x, y) in enumerate(zip(a, b)) if x != y]
    if changes:
        allowed = timestamp_offsets(a)
        assert allowed == timestamp_offsets(b)
        assert set(changes) <= allowed, (old.name, changes)
    rows.append(dict(file=old.name, bytes=len(a), changedBytes=len(changes),
                     kind='PE-timestamps-only' if changes else 'byte-identical',
                     beforeSha256=hashlib.sha256(a).hexdigest(),
                     afterSha256=hashlib.sha256(b).hexdigest(), offsets=changes))
assert len(rows) == 14
(root / 'verified-comparison.json').write_text(json.dumps(rows, indent=2)+'\n')
print(json.dumps({kind: sum(row['kind'] == kind for row in rows)
                  for kind in ('byte-identical', 'PE-timestamps-only')}))
