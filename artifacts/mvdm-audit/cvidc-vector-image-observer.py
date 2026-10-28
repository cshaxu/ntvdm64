"""Compare original C-VID vector relocations with the released worker image."""
import csv
import hashlib
import json
import runpy
import struct
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BUILD = Path('build/.ninja/msvc-x86-release/worker')


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    h = runpy.run_path(str(A / 'host-tail-image-observer.py'))
    image_path = Path('assets/release/ntvdm.exe')
    image, cached = h['Image'](image_path), h['Image'](BUILD / 'ntvdm.exe')
    assert image.base == cached.base and image.code == cached.code
    assert all(image.data[s:e] == cached.data[s:e] for s, e, _ in image.code)
    relative = 'softpc.new/base/cvidc/evidfunc.c'
    source = Path('src/mvdm') / relative
    original = Path('O:/repos.external/OpenNT/base/mvdm') / relative
    original45 = Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm') / relative
    frozen = {r['relative_path']: r for r in csv.DictReader(
        (A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig', newline=''))}[relative]
    assert sha(source) == sha(original) == sha(original45) == frozen['current_sha256'] == frozen['opennt_sha256'] == frozen['opennt45_sha256']
    assert frozen['dos_catalog_rows'] == '1' and frozen['wow_catalog_rows'] == '2'
    obj_path = BUILD / 'obj/cvidc/evidfunc.obj'
    graph = (BUILD / 'build.ninja').read_text().replace('$:', ':')
    edge = next(line for line in graph.splitlines() if line.startswith('build obj/cvidc/evidfunc.obj:'))
    archive = next(line for line in graph.splitlines() if line.startswith('build original-softpc-cvidc.lib:'))
    assert source.resolve().as_posix().lower() in edge.lower() and 'obj/cvidc/evidfunc.obj' in archive.split()
    deps = subprocess.check_output(['ninja', '-C', str(BUILD), '-t', 'deps'], text=True)
    dep = next(line for line in deps.splitlines() if line.startswith('obj/cvidc/evidfunc.obj:'))
    assert '(VALID)' in dep and '(STALE)' not in dep
    obj = h['Coff'](obj_path)
    symbols = h['symbols']((BUILD / 'ntvdm.exe.map').read_text())
    tables = sorted((name, offset, section) for name, offset, section in obj.symbols.values()
                    if name.endswith('_evid') and section > 0)
    assert len(tables) == 14
    by_section = sorted(tables, key=lambda x: (x[2], x[1]))
    assert all(section == 3 for _, _, section in by_section)
    body, relocs = obj.sections[2]
    assert len(body) == 2768 and len(relocs) == 692
    results = []
    for index, (name, start, section) in enumerate(by_section):
        end = by_section[index + 1][1] if index + 1 < len(by_section) else len(body)
        assert (end - start) % 4 == 0
        table = [s for s in symbols if s['name'] == name and s['owner'].lower() == 'original-softpc-cvidc:evidfunc.obj']
        assert len(table) == 1
        actual = h['data_at'](image, table[0]['va'], end - start)
        selected = [(p - start, target, kind) for p, target, kind in relocs if start <= p < end]
        assert {p for p, _, _ in selected} == set(range(0, end - start, 4))
        entries = []
        for offset, target, kind in selected:
            assert kind == 6
            addend = struct.unpack_from('<I', body, start + offset)[0]
            value = struct.unpack_from('<I', actual, offset)[0]
            targets = [s for s in symbols if s['name'] == target and (s['va'] + addend) & 0xffffffff == value]
            assert targets, (name, offset, target, value)
            entries.append(dict(index=offset // 4, symbol=target, va=value,
                                owners=sorted({s['owner'] for s in targets})))
        results.append(dict(name=name, va=table[0]['va'], entries=entries))
    return dict(result='pass', base='27a9d2e63', relative_path=relative,
        sha256=sha(source), original_path=str(original), original45_path=str(original45),
        dos_catalog_rows=1, wow_catalog_rows=2, wow_source_ids=frozen['wow_source_ids'],
        compile_edge=edge, dependency_header=dep, tables=results, inputs=[dict(path=str(p), sha256=sha(p)) for p in
            (image_path, BUILD / 'ntvdm.exe', BUILD / 'ntvdm.exe.map', BUILD / 'build.ninja', obj_path)],
        scope='Initial linked vector values only; actual mode choice and VRAM behavior remain open')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
