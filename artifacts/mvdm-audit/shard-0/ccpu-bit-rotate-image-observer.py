"""Bind two original CCPU instruction bodies to the current released worker."""
import csv
import hashlib
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
B = Path('build/.ninja/msvc-x86-release/worker')
def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()
def inspect():
    helper = runpy.run_path(str(A / 'host-tail-image-observer.py'))
    frozen = {r['relative_path']: r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    graph = (B / 'build.ninja').read_text().replace('$:', ':')
    archive = next(line for line in graph.splitlines() if line.startswith('build original-ccpu386.lib:'))
    image = helper['Image'](Path('assets/release/ntvdm.exe'))
    cached = helper['Image'](B / 'ntvdm.exe')
    assert image.code == cached.code and all(image.data[a:b] == cached.data[a:b] for a, b, _ in image.code)
    symbols = helper['symbols']((B / 'ntvdm.exe.map').read_text())
    result = []
    for stem in ('btc', 'rcl'):
        rel = 'softpc.new/base/ccpu386/' + stem + '.c'
        src = Path('src/mvdm') / rel
        original = Path('O:/repos.external/OpenNT/base/mvdm') / rel
        other = Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm') / rel
        row = frozen[rel]
        assert sha(src) == sha(original) == sha(other) == row['current_sha256'] == row['opennt_sha256'] == row['opennt45_sha256']
        edge = next(line for line in graph.splitlines() if line.startswith('build obj/ccpu/' + stem + '.obj:'))
        assert src.resolve().as_posix().lower() in edge.lower() and ('obj/ccpu/' + stem + '.obj') in archive.split()
        obj = helper['Coff'](B / ('obj/ccpu/' + stem + '.obj'))
        sym = next(x for x in symbols if x['name'] == '_' + stem.upper() and x['owner'] == 'original-ccpu386:' + stem + '.obj')
        result.append(dict(path=rel, sha256=sha(src), edge=edge, binding=helper['bind'](image, obj, sym),
                           wow_source_ids=row['wow_source_ids']))
    return dict(result='pass', sources=result, image_sha256=sha('assets/release/ntvdm.exe'),
                map_sha256=sha(B / 'ntvdm.exe.map'), graph_sha256=sha(B / 'build.ninja'),
                limits='Source and selected instruction body binding only; actual operand, flags, faults and guest execution remain separate.')
if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
