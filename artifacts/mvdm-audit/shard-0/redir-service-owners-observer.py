"""Bind selected original redirector service owners to released DLL code."""
import csv
import hashlib
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
B = Path('build/.ninja/msvc-x86-release/worker')
ROOTS = (Path('src/mvdm'), Path('O:/repos.external/OpenNT/base/mvdm'),
         Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
STEMS = ('vrinit', 'vrmisc', 'vrmslot', 'vrputil')


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()


def inspect():
    frozen = {r['relative_path']: r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    h = runpy.run_path(str(A / 'host-tail-image-observer.py'))
    image, cached = h['Image'](Path('assets/release/VDMREDIR.DLL')), h['Image'](B / 'VDMREDIR.dll')
    assert image.base == cached.base and image.code == cached.code
    assert all(image.data[a:b] == cached.data[a:b] for a, b, _ in image.code)
    graph = (B / 'build.ninja').read_text().replace('$:', ':')
    link = next(x for x in graph.splitlines() if x.startswith('build VDMREDIR.dll |'))
    symbols = h['symbols']((A / 'shard-3/redir-private-map.txt').read_text())
    entries = []
    for stem in STEMS:
        rel = 'vdmredir/' + stem + '.c'
        hashes = [sha(root / rel) for root in ROOTS]
        assert hashes == [frozen[rel]['current_sha256'], frozen[rel]['opennt_sha256'], frozen[rel]['opennt45_sha256']]
        target = 'obj/redir/' + stem + '.obj'
        assert target in link.split()
        edge = next(x for x in graph.splitlines() if x.startswith('build ' + target + ':'))
        own = [s for s in symbols if s['owner'].lower() == stem + '.obj' and s['function']]
        assert own
        obj = h['Coff'](B / target)
        bindings = [h['bind'](image, obj, s) for s in own]
        entries.append(dict(path=rel, hashes=hashes, edge=edge, object_sha256=sha(B / target), bindings=bindings))
    return dict(result='pass', sources=entries, release_sha256=sha('assets/release/VDMREDIR.DLL'),
        cached_sha256=sha(B / 'VDMREDIR.dll'), map_sha256=sha(A / 'shard-3/redir-private-map.txt'),
        limits='Masked object body bindings only; no provider availability, guest request or lifecycle proof.')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
