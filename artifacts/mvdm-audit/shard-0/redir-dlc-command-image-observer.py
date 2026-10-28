"""Bind original DLC command owner and event references to released DLL."""
import csv
import hashlib
import json
import runpy
import struct
from pathlib import Path

A = Path('artifacts/mvdm-audit')
B = Path('build/.ninja/msvc-x86-release/worker')
REL = 'vdmredir/vrdlc5c.c'
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
    map_path = A / 'shard-3/redir-private-map.txt'
    symbols = h['symbols'](map_path.read_text())
    own = [s for s in symbols if s['owner'].lower() == 'vrdlc5c.obj' and s['function'] and not s['name'].startswith('$')]
    assert len(own) == 17
    objpath = B / 'obj/redir/vrdlc5c.obj'
    obj = h['Coff'](objpath)
    bindings = [h['bind'](image, obj, s) for s in own]
    init = next(s for s in own if s['name'] == '_InitializeAdapterSupport')
    body, relocs = obj.function(init['name'])
    fields = []
    for offset, name in ((98, '_DlcWorkerEvent'), (118, '_DlcWorkerEvent'), (124, '_DlcWorkerCompletionEvent')):
        assert (offset, name, 6) in relocs
        actual = struct.unpack('<I', h['data_at'](image, init['va'] + offset, 4))[0]
        target = next(s for s in symbols if s['name'] == name)
        assert actual == (target['va'] + struct.unpack_from('<I', body, offset)[0]) & 0xffffffff
        fields.append(dict(offset=offset, name=name, target=actual))
    assert fields[0]['target'] == fields[1]['target'] != fields[2]['target']
    graph = (B / 'build.ninja').read_text().replace('$:', ':')
    link = next(x for x in graph.splitlines() if x.startswith('build VDMREDIR.dll |'))
    assert 'obj/redir/vrdlc5c.obj' in link.split()
    return dict(result='pass', source=REL, hashes=hashes, bindings=bindings, event_fields=fields,
        object_sha256=sha(objpath), map_sha256=sha(map_path), release_sha256=sha('assets/release/VDMREDIR.DLL'),
        cached_sha256=sha(B / 'VDMREDIR.dll'), limits='Static image and relocation binding; provider reachability and event failures not exercised.')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
