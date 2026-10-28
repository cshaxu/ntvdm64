"""Bind original WOW keyboard thunk bodies to released WOW32 code."""
import csv
import hashlib
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
B = Path('build/.ninja/msvc-x86-release/wow32')
REL = 'wow32/wkbman.c'
ROOTS = (Path('src/mvdm'), Path('O:/repos.external/OpenNT/base/mvdm'),
         Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    frozen = next(r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig')) if r['relative_path'] == REL)
    hashes = [sha(root / REL) for root in ROOTS]
    assert hashes == [frozen['current_sha256'], frozen['opennt_sha256'], frozen['opennt45_sha256']]
    h = runpy.run_path(str(A / 'host-tail-image-observer.py'))
    image, cached = h['Image'](Path('assets/release/WOW32.DLL')), h['Image'](B / 'wow32.dll')
    assert image.base == cached.base and image.code == cached.code
    assert all(image.data[a:b] == cached.data[a:b] for a, b, _ in image.code)
    graph = (B / 'build.ninja').read_text().replace('$:', ':')
    edge = next(x for x in graph.splitlines() if x.startswith('build obj/wkbman.obj:'))
    link = next(x for x in graph.splitlines() if x.startswith('build wow32.dll | wow32.lib:'))
    assert 'obj/wkbman.obj' in link.split()
    symbols = [s for s in h['symbols']((B / 'wow32.dll.map').read_text())
               if s['owner'].lower() == 'wkbman.obj' and s['name'].startswith('_WKB32') and s['function']]
    assert len(symbols) == 11
    obj = h['Coff'](B / 'obj/wkbman.obj')
    bindings = [h['bind'](image, obj, symbol) for symbol in symbols]
    return dict(result='pass', source=REL, hashes=hashes, compile_edge=edge, bindings=bindings,
        object_sha256=sha(B / 'obj/wkbman.obj'), map_sha256=sha(B / 'wow32.dll.map'),
        release_sha256=sha('assets/release/WOW32.DLL'), cached_sha256=sha(B / 'wow32.dll'),
        limits='Object body bindings only; guest pointer, keyboard layouts, callback, failures and teardown remain untested.')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
