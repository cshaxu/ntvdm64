"""Bind the original bidirectional scrollbar message owners to WOW32."""
import csv
import hashlib
import json
import runpy
import struct
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit')
B = Path('build/.ninja/msvc-x86-release/wow32')
M = Path('src/mvdm')
REL = 'wow32/wmsgsbm.c'
ORIGINALS = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    h = runpy.run_path(str(A / 'host-tail-image-observer.py'))
    frozen = next(r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))
                  if r['relative_path'] == REL)
    source = M / REL
    assert sha(source) == sha(ORIGINALS[0] / REL) == sha(ORIGINALS[1] / REL)
    assert sha(source) == frozen['current_sha256'] == frozen['opennt_sha256'] == frozen['opennt45_sha256']
    assert frozen['dos_catalog_rows'] == '1' and frozen['wow_catalog_rows'] == '2'
    graph = (B / 'build.ninja').read_text().replace('$:', ':')
    edge = next(line for line in graph.splitlines() if line.startswith('build obj/wmsgsbm.obj:'))
    link = next(line for line in graph.splitlines() if line.startswith('build wow32.dll | wow32.lib:'))
    assert source.resolve().as_posix().lower() in edge.lower() and 'obj/wmsgsbm.obj' in link.split()
    deps = subprocess.check_output(['ninja', '-C', str(B), '-t', 'deps'], text=True)
    dep = next(line for line in deps.splitlines() if line.startswith('obj/wmsgsbm.obj:'))
    assert '(VALID)' in dep and '(STALE)' not in dep
    image_path = Path('assets/release/WOW32.DLL')
    image, cached = h['Image'](image_path), h['Image'](B / 'wow32.dll')
    assert image.base == cached.base and image.code == cached.code
    assert all(image.data[a:b] == cached.data[a:b] for a, b, _ in image.code)
    syms = h['symbols']((B / 'wow32.dll.map').read_text())
    expected = [('_ThunkSBMMsg16@4', 'wmsgsbm'), ('_UnThunkSBMMsg16@4', 'wmsgsbm'),
                ('_WM32SBMControl@4', 'wcntl32'), ('_WM32SBMGetRange@4', 'wcntl32'),
                ('_WM32SBMSetRange@4', 'wcntl32')]
    mapped = {}
    for name, owner in expected:
        symbol = next(s for s in syms if s['name'] == name and s['owner'].lower() == owner + '.obj' and s['function'])
        mapped[name] = h['bind'](image, h['Coff'](B / ('obj/' + owner + '.obj')), symbol)
    entries = []
    requests = [('_apfnThunkMsg16', 'wmsg16', 7, '_ThunkSBMMsg16@4'),
                ('_apfnUnThunkMsg16', 'wmsg16', 7, '_UnThunkSBMMsg16@4')]
    for index, name in ((0xe0, '_WM32SBMControl@4'), (0xe1, '_WM32SBMControl@4'),
                        (0xe2, '_WM32SBMSetRange@4'), (0xe3, '_WM32SBMGetRange@4'),
                        (0xe4, '_WM32SBMControl@4'), (0xe6, '_WM32SBMSetRange@4')):
        requests.append(('_aw32Msg', 'wmtbl32', index, name))
    for table_name, owner, index, target in requests:
        obj = h['Coff'](B / ('obj/' + owner + '.obj'))
        _, start, section = next(s for s in obj.symbols.values() if s[0] == table_name and s[2] > 0)
        body, relocs = obj.sections[section - 1]
        offset = start + 4 * index
        assert [r for r in relocs if r[0] == offset] == [(offset, target, 6)]
        table = next(s for s in syms if s['name'] == table_name and s['owner'].lower() == owner + '.obj')
        value = struct.unpack('<I', h['data_at'](image, table['va'] + 4 * index, 4))[0]
        addend = struct.unpack_from('<I', body, offset)[0]
        assert value == (mapped[target]['va'] + addend) & 0xffffffff
        entries.append(dict(table=table_name, index=index, target=target, image_pointer=value))
    inputs = [image_path, B / 'wow32.dll', B / 'wow32.dll.map', B / 'build.ninja'] + [
        B / ('obj/' + n + '.obj') for n in ('wmsgsbm', 'wmsg16', 'wmtbl32', 'wcntl32')]
    return dict(result='pass', relative_path=REL, sha256=sha(source), original_path=str(ORIGINALS[0] / REL),
        dos_catalog_rows=1, wow_catalog_rows=2, wow_source_ids=frozen['wow_source_ids'],
        compile_edge=edge, dependency_header=dep, functions=list(mapped.values()), table_entries=entries,
        inputs=[dict(path=str(p), sha256=sha(p)) for p in inputs],
        scope='Initial bidirectional scrollbar message bindings; not guest message execution or lifecycle proof')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
