"""Bind selected original DLC event/post routines to release VDMREDIR."""
import csv
import hashlib
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
B = Path('build/.ninja/msvc-x86-release/worker')
REL = 'vdmredir/vrdlcpst.c'
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
    graph = (B / 'build.ninja').read_text().replace('$:', ':')
    link = next(x for x in graph.splitlines() if x.startswith('build VDMREDIR.dll |'))
    target = 'obj/redir/vrdlcpst.obj'
    assert target in link.split()
    edge = next(x for x in graph.splitlines() if x.startswith('build ' + target + ':'))
    symbols = [s for s in h['symbols']((A / 'shard-3/redir-private-map.txt').read_text())
               if s['owner'].lower() == 'vrdlcpst.obj' and s['function'] and not s['name'].startswith('$')]
    assert len(symbols) == 11
    obj = h['Coff'](B / target)
    return dict(result='pass', source=REL, hashes=hashes, compile_edge=edge,
        bindings=[h['bind'](image, obj, s) for s in symbols], object_sha256=sha(B / target),
        map_sha256=sha(A / 'shard-3/redir-private-map.txt'), release_sha256=sha('assets/release/VDMREDIR.DLL'),
        cached_sha256=sha(B / 'VDMREDIR.dll'), limits='Object bindings only; actual DLC provider, interrupts, errors and teardown remain unverified.')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
