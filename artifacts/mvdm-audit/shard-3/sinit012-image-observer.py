"""Bind the shard-3 SINIT012 object to the currently released worker."""
import csv
import hashlib
import json
import runpy
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit')
B = Path('build/.ninja/msvc-x86-release/worker')
REL = 'softpc.new/base/cvidc/sinit012.c'


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    h = runpy.run_path(str(A / 'host-tail-image-observer.py'))
    frozen = next(r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig')) if r['relative_path'] == REL)
    src = Path('src/mvdm') / REL
    first = Path('O:/repos.external/OpenNT/base/mvdm') / REL
    second = Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm') / REL
    assert sha(src) == sha(first) == sha(second) == frozen['current_sha256'] == frozen['opennt_sha256'] == frozen['opennt45_sha256']
    assert frozen['dos_catalog_rows'] == '1' and frozen['wow_catalog_rows'] == '2'
    graph = (B / 'build.ninja').read_text().replace('$:', ':')
    edge = next(s for s in graph.splitlines() if s.startswith('build obj/cvidc/SINIT012.obj:'))
    archive = next(s for s in graph.splitlines() if s.startswith('build original-softpc-cvidc.lib:'))
    assert src.resolve().as_posix().lower() in edge.lower() and 'obj/cvidc/SINIT012.obj' in archive.split()
    dep = next(s for s in subprocess.check_output(['ninja','-C',str(B),'-t','deps'],text=True).splitlines() if s.startswith('obj/cvidc/SINIT012.obj:'))
    assert '(VALID)' in dep and '(STALE)' not in dep
    image_path = Path('assets/release/ntvdm.exe')
    image = h['Image'](image_path)
    obj = h['Coff'](B / 'obj/cvidc/SINIT012.obj')
    syms = [s for s in h['symbols']((B / 'ntvdm.exe.map').read_text()) if s['function'] and s['owner'].lower() == 'original-softpc-cvidc:sinit012.obj']
    assert len(syms) == 33
    mapped = [h['bind'](image, obj, s) for s in syms]
    return dict(result='pass', relative_path=REL, source_sha256=sha(src),
        wow_source_ids=frozen['wow_source_ids'], dos_catalog_rows=1, wow_catalog_rows=2,
        edge=edge, dependency_header=dep, functions=mapped,
        inputs=[dict(path=str(x), sha256=sha(x)) for x in (src,first,second,image_path,
            B / 'ntvdm.exe.map',B / 'build.ninja',B / 'obj/cvidc/SINIT012.obj')],
        scope='Code identity excluding relocation words; no generated getter execution or lifecycle claim')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
