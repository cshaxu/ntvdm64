"""Bind selected original SINIT011 accessors to the released worker."""
import csv
import hashlib
import json
import runpy
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit')
REL = 'softpc.new/base/cvidc/sinit011.c'


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    h = runpy.run_path(str(A / 'host-tail-image-observer.py'))
    build = h['BUILD']
    with (A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig', newline='') as stream:
        record = next(r for r in csv.DictReader(stream) if r['relative_path'] == REL)
    source = Path('src/mvdm') / REL
    first = Path('O:/repos.external/OpenNT/base/mvdm') / REL
    second = Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm') / REL
    assert sha(source) == sha(first) == sha(second) == record['current_sha256'] == record['opennt_sha256'] == record['opennt45_sha256']
    assert record['dos_catalog_rows'] == '1' and record['wow_catalog_rows'] == '2'
    graph = (build / 'build.ninja').read_text().replace('$:', ':')
    target = 'obj/cvidc/SINIT011.obj'
    edge = next(s for s in graph.splitlines() if s.startswith('build ' + target + ':'))
    archive = next(s for s in graph.splitlines() if s.startswith('build original-softpc-cvidc.lib:'))
    assert source.resolve().as_posix().lower() in edge.lower() and target in archive.split()
    deps = subprocess.check_output(['ninja', '-C', str(build), '-t', 'deps'], text=True)
    dependency = next(s for s in deps.splitlines() if s.startswith(target + ':'))
    assert '(VALID)' in dependency and '(STALE)' not in dependency
    image_path = Path('assets/release/ntvdm.exe')
    image, obj = h['Image'](image_path), h['Coff'](build / target)
    syms = [s for s in h['symbols']((build / 'ntvdm.exe.map').read_text())
            if s['function'] and s['owner'].lower() == 'original-softpc-cvidc:sinit011.obj']
    assert len(syms) == 21
    return dict(result='pass', relative_path=REL, source_sha256=sha(source),
        wow_source_ids=record['wow_source_ids'], edge=edge, dependency_header=dependency,
        functions=[h['bind'](image, obj, s) for s in syms],
        inputs=[dict(path=str(p), sha256=sha(p)) for p in
            (source, first, second, image_path, build / 'ntvdm.exe.map', build / 'build.ninja', build / target)],
        scope='Initial code identity excluding relocation words; no accessor behavior or lifetime claim')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
