"""Verify selected redirector dispatch entries against the released DLL."""
import csv
import hashlib
import json
import runpy
import struct
from pathlib import Path

A = Path('artifacts/mvdm-audit')
B = Path('build/.ninja/msvc-x86-release/worker')
REL = 'vdmredir/vrdisp.c'
ROOTS = (Path('src/mvdm'), Path('O:/repos.external/OpenNT/base/mvdm'),
         Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()


def inspect():
    frozen = next(r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig')) if r['relative_path'] == REL)
    hashes = [sha(root / REL) for root in ROOTS]
    assert hashes == [frozen['current_sha256'], frozen['opennt_sha256'], frozen['opennt45_sha256']]
    h = runpy.run_path(str(A / 'host-tail-image-observer.py'))
    image, cached = h['Image'](Path('assets/release/VDMREDIR.DLL')), h['Image'](B / 'VDMREDIR.dll')
    assert image.base == cached.base and image.code == cached.code
    assert all(image.data[a:b] == cached.data[a:b] for a, b, _ in image.code)
    symbols = h['symbols']((A / 'shard-3/redir-private-map.txt').read_text())
    obj = h['Coff'](B / 'obj/redir/vrdisp.obj')
    dispatch = next(s for s in symbols if s['name'] == '_VrDispatch' and s['owner'].lower() == 'vrdisp.obj')
    binding = h['bind'](image, obj, dispatch)
    table = next(s for s in symbols if s['name'] == '_VrDispatchTable' and s['owner'].lower() == 'vrdisp.obj')
    definition = next(s for s in obj.symbols.values() if s[0] == '_VrDispatchTable' and s[2] > 0)
    body, relocs = obj.sections[definition[2] - 1]
    assert len(body) == 200 and len(relocs) == 50
    entries = []
    for index in range(50):
        offset = definition[1] + index * 4
        record = [r for r in relocs if r[0] == offset]
        assert len(record) == 1 and record[0][2] == 6
        target = next(s for s in symbols if s['name'] == record[0][1])
        actual = struct.unpack('<I', h['data_at'](image, table['va'] + index * 4, 4))[0]
        addend = struct.unpack_from('<i', body, offset)[0]
        assert actual == target['va'] + addend, (index, record, actual, target)
        entries.append(dict(index=index, name=target['name'], owner=target['owner'], va=actual))
    return dict(result='pass', source=REL, hashes=hashes, dispatch_binding=binding, entries=entries,
        object_sha256=sha(B / 'obj/redir/vrdisp.obj'), map_sha256=sha(A / 'shard-3/redir-private-map.txt'),
        release_sha256=sha('assets/release/VDMREDIR.DLL'), cached_sha256=sha(B / 'VDMREDIR.dll'),
        limits='Original source and static dispatch binding only; no guest or network execution proof.')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
