"""Bind selected original WOW shell, hook and subclass owners to released code."""
import csv
import hashlib
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
B = Path('build/.ninja/msvc-x86-release/wow32')
ROOTS = (Path('src/mvdm'), Path('O:/repos.external/OpenNT/base/mvdm'),
         Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
STEMS = ('wshell', 'wsubcls', 'wuhook')


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()


def inspect():
    frozen = {r['relative_path']: r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    h = runpy.run_path(str(A / 'host-tail-image-observer.py'))
    image, cached = h['Image'](Path('assets/release/WOW32.DLL')), h['Image'](B / 'wow32.dll')
    assert image.base == cached.base and image.code == cached.code
    assert all(image.data[a:b] == cached.data[a:b] for a, b, _ in image.code)
    graph = (B / 'build.ninja').read_text().replace('$:', ':')
    link = next(x for x in graph.splitlines() if x.startswith('build wow32.dll |'))
    symbols = h['symbols']((B / 'wow32.dll.map').read_text())
    sources = []
    for stem in STEMS:
        rel = 'wow32/' + stem + '.c'
        hashes = [sha(root / rel) for root in ROOTS]
        assert hashes == [frozen[rel]['current_sha256'], frozen[rel]['opennt_sha256'], frozen[rel]['opennt45_sha256']]
        target = 'obj/' + stem + '.obj'
        assert target in link.split()
        edge = next(x for x in graph.splitlines() if x.startswith('build ' + target + ':'))
        own = [s for s in symbols if s['owner'].lower() == stem + '.obj' and s['function'] and not s['name'].startswith('$')]
        assert own
        obj = h['Coff'](B / target)
        sources.append(dict(path=rel, hashes=hashes, edge=edge, object_sha256=sha(B / target),
                            bindings=[h['bind'](image, obj, s) for s in own]))
    return dict(result='pass', sources=sources, release_sha256=sha('assets/release/WOW32.DLL'),
        cached_sha256=sha(B / 'wow32.dll'), map_sha256=sha(B / 'wow32.dll.map'),
        limits='Object body bindings only; guest dispatch, callback reentrancy, handles and teardown remain unverified.')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
