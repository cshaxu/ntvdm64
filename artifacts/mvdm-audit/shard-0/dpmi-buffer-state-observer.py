"""Bind selected original DPMI translation buffer and state owners."""
import csv
import hashlib
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit'); B = Path('build/.ninja/msvc-x86-release/worker')
def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()
def inspect():
    h = runpy.run_path(str(A / 'host-tail-image-observer.py'))
    rows = {r['relative_path']: r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    graph = (B / 'build.ninja').read_text().replace('$:', ':')
    archive = next(x for x in graph.splitlines() if x.startswith('build original-mvdm-dpmi32.lib:'))
    image = h['Image'](Path('assets/release/ntvdm.exe')); cached = h['Image'](B / 'ntvdm.exe')
    assert image.code == cached.code and all(image.data[a:b] == cached.data[a:b] for a,b,_ in image.code)
    symbols = h['symbols']((B / 'ntvdm.exe.map').read_text())
    sources = []
    for stem, names in [('buffer', ('_DpmiAllocateBuffer', '_DpmiMapAndCopyBuffer', '_DpmiUnmapAndCopyBuffer')),
                        ('savestat', ('_DpmiSaveSegmentsAndStack', '_DpmiRestoreSegmentsAndStack'))]:
        rel = 'dpmi32/' + stem + '.c'; src = Path('src/mvdm') / rel; row = rows[rel]
        original = Path('O:/repos.external/OpenNT/base/mvdm') / rel
        alternate = Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm') / rel
        assert sha(src) == sha(original) == sha(alternate) == row['current_sha256'] == row['opennt_sha256'] == row['opennt45_sha256']
        edge = next(x for x in graph.splitlines() if x.startswith('build obj/dpmi/' + stem + '.obj:'))
        assert src.resolve().as_posix().lower() in edge.lower() and 'obj/dpmi/' + stem + '.obj' in archive.split()
        obj = h['Coff'](B / ('obj/dpmi/' + stem + '.obj')); bindings = []
        for name in names:
            sym = next(x for x in symbols if x['name'] == name and x['owner'] == 'original-mvdm-dpmi32:' + stem + '.obj')
            bindings.append(h['bind'](image, obj, sym))
        sources.append(dict(path=rel, sha256=sha(src), wow_source_ids=row['wow_source_ids'], edge=edge, bindings=bindings))
    return dict(result='pass', sources=sources, image_sha256=sha('assets/release/ntvdm.exe'),
                map_sha256=sha(B / 'ntvdm.exe.map'), graph_sha256=sha(B / 'build.ninja'),
                limits='Selected source/object/image bindings, not guest mapping, nested state, failures or cleanup.')
if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
