"""Bind pending selected C-VID generated objects to the released worker."""
import csv
import hashlib
import json
import re
import runpy
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BUILD = Path('build/.ninja/msvc-x86-release/worker')
NUMBERS = (1, 2, 6, 8, 9, 11, 13, 15, 17, 18, 19, 20, 22, 23, 26, 28, 29)


def digest(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    h = runpy.run_path(str(A / 'host-tail-image-observer.py'))
    frozen = {r['relative_path']: r for r in csv.DictReader(
        (A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig', newline=''))}
    graph = (BUILD / 'build.ninja').read_text().replace('$:', ':')
    archive = next(line for line in graph.splitlines() if line.startswith('build original-softpc-cvidc.lib:'))
    deps = subprocess.check_output(['ninja', '-C', str(BUILD), '-t', 'deps'], text=True)
    blocks = {b.split(':', 1)[0].lower(): b for b in re.split(r'(?m)(?=^obj/)', deps) if b.startswith('obj/')}
    image = h['Image'](Path('assets/release/ntvdm.exe'))
    cached = h['Image'](BUILD / 'ntvdm.exe')
    assert image.base == cached.base and image.code == cached.code
    assert all(image.data[s:e] == cached.data[s:e] for s, e, _ in image.code)
    symbols = h['symbols']((BUILD / 'ntvdm.exe.map').read_text())
    table_path = Path('src/mvdm/softpc.new/base/cvidc/evidfunc.c')
    table = table_path.read_text(encoding='latin1')
    sources = []
    for number in NUMBERS:
        stem = f'SEVID{number:03}'
        relative = f'softpc.new/base/cvidc/{stem.lower()}.c'
        source = Path('src/mvdm') / relative
        original = Path('O:/repos.external/OpenNT/base/mvdm') / relative
        original45 = Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm') / relative
        record = frozen[relative]
        sha = digest(source)
        assert sha == digest(original) == digest(original45) == record['current_sha256'] == record['opennt_sha256'] == record['opennt45_sha256']
        assert record['dos_catalog_rows'] == '1' and record['wow_catalog_rows'] == '2'
        target = f'obj/cvidc/{stem}.obj'
        edge = next(line for line in graph.splitlines() if line.startswith('build ' + target + ':'))
        assert source.resolve().as_posix().lower() in edge.lower() and target in archive.split()
        block = blocks[target.lower()]
        assert '(VALID)' in block.splitlines()[0] and '(STALE)' not in block
        obj = h['Coff'](BUILD / target)
        mapped = [s for s in symbols if s['function'] and s['owner'].lower() == ('original-softpc-cvidc:' + stem + '.obj').lower()]
        assert mapped
        bindings = [h['bind'](image, obj, symbol) for symbol in mapped]
        table_names = [s['name'][1:] for s in mapped if s['name'].startswith('_S_')
                       and re.search(r'\b' + re.escape(s['name'][1:]) + r'\b', table)]
        sources.append(dict(relative_path=relative, original_path=str(original), original45_path=str(original45),
            sha256=sha, dos_catalog_rows=1, wow_catalog_rows=2,
            wow_source_ids=record['wow_source_ids'], object_sha256=digest(BUILD / target),
            compile_edge=edge, dependency_header=block.splitlines()[0],
            table_names=table_names, functions=bindings))
    return dict(result='pass', base='626f77507', sources=sources,
        image_sha256=digest('assets/release/ntvdm.exe'), map_sha256=digest(BUILD / 'ntvdm.exe.map'),
        graph_sha256=digest(BUILD / 'build.ninja'), dependency_sha256=hashlib.sha256(deps.encode()).hexdigest().upper(),
        table_sha256=digest(table_path), scope='Object code and source table mentions, not executed plane/latch behavior')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
