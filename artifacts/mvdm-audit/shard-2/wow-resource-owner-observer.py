"""Read-only binding of original WOW16 resource owners and selected callers."""
import csv
import hashlib
import json
import runpy
import struct
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BUILD = Path('build/.ninja/msvc-x86-release/wow32')
ROOT = Path('src/mvdm')
ORIGINALS = (Path('O:/repos.external/OpenNT/base/mvdm'),
             Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    h = runpy.run_path(str(A / 'host-tail-image-observer.py'))
    frozen = {r['relative_path']: r for r in csv.DictReader(
        (A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig', newline=''))}
    paths = ('wow32/wres16.c', 'wow32/wres16.h')
    sources = []
    for relative in paths:
        f = frozen[relative]
        digest = sha(ROOT / relative)
        assert digest == sha(ORIGINALS[0] / relative) == sha(ORIGINALS[1] / relative)
        assert digest == f['current_sha256'] == f['opennt_sha256'] == f['opennt45_sha256']
        assert f['dos_catalog_rows'] == '1' and f['wow_catalog_rows'] == '2'
        sources.append(dict(relative_path=relative, sha256=digest, original_path=str(ORIGINALS[0] / relative),
                            wow_source_ids=f['wow_source_ids'], dos_catalog_rows=1, wow_catalog_rows=2))
    graph = (BUILD / 'build.ninja').read_text().replace('$:', ':')
    edge = next(line for line in graph.splitlines() if line.startswith('build obj/wres16.obj:'))
    link = next(line for line in graph.splitlines() if line.startswith('build wow32.dll | wow32.lib:'))
    assert (ROOT / paths[0]).resolve().as_posix().lower() in edge.lower() and 'obj/wres16.obj' in link.split()
    deps = subprocess.check_output(['ninja', '-C', str(BUILD), '-t', 'deps'], text=True)
    block = next(b for b in deps.split('\n\n') if b.lower().startswith('obj/wres16.obj:'))
    assert '(VALID)' in block.splitlines()[0] and '(STALE)' not in block
    assert 'wres16.h' in block.lower()
    image_path = Path('assets/release/WOW32.DLL')
    image, cached = h['Image'](image_path), h['Image'](BUILD / 'wow32.dll')
    assert image.base == cached.base and image.code == cached.code
    assert all(image.data[s:e] == cached.data[s:e] for s, e, _ in image.code)
    syms = h['symbols']((BUILD / 'wow32.dll.map').read_text())
    obj = h['Coff'](BUILD / 'obj/wres16.obj')
    funcs = [h['bind'](image, obj, s) for s in syms if s['function'] and s['owner'].lower() == 'wres16.obj']
    assert len(funcs) == 10
    edges = []
    requests = [('_W32FindResource@16', 'wres32', '_FindResource16@12'),
                ('_W32FreeResource@8', 'wres32', '_FreeResource16@4'),
                ('_W32LoadResource@8', 'wres32', '_LoadResource16@8'),
                ('_W32LockResource@8', 'wres32', '_LockResource16@4'),
                ('_W32SizeofResource@8', 'wres32', '_SizeofResource16@8'),
                ('_W32UnlockResource@8', 'wres32', '_UnlockResource16@4'),
                ('_WowOriginalFreeTask@4', 'wkman', '_DestroyRes16@4'),
                ('_WU32DialogBoxParam@4', 'wudlg', '_ConvertDialog16@16'),
                ('_GetTemplate16@12', 'wcommdlg', '_ConvertDialog16@16')]
    for caller, owner, target in requests:
        symbol = next(s for s in syms if s['name'] == caller and s['owner'].lower() == owner + '.obj' and s['function'])
        dest = next(s for s in syms if s['name'] == target and s['owner'].lower() == 'wres16.obj' and s['function'])
        caller_obj = h['Coff'](BUILD / ('obj/' + owner + '.obj'))
        bound = h['bind'](image, caller_obj, symbol)
        body, relocs = caller_obj.function(caller)
        offsets = [p for p, n, k in relocs if n == target and k == 20]
        assert offsets
        for offset in offsets:
            assert body[offset - 1] in (0xe8, 0xe9)
            displacement = struct.unpack('<i', h['data_at'](image, symbol['va'] + offset, 4))[0]
            addend = struct.unpack_from('<i', body, offset)[0]
            assert symbol['va'] + offset + 4 + displacement == dest['va'] + addend
        edges.append(dict(caller=caller, target=target, offsets=offsets, binding=bound))
    inputs = [image_path, BUILD / 'wow32.dll', BUILD / 'wow32.dll.map', BUILD / 'build.ninja',
              BUILD / 'obj/wres16.obj', BUILD / 'obj/wres32.obj', BUILD / 'obj/wkman.obj',
              BUILD / 'obj/wudlg.obj', BUILD / 'obj/wcommdlg.obj']
    return dict(result='pass', sources=sources, compile_edge=edge, dependency_header=block.splitlines()[0],
                functions=funcs, edges=edges, inputs=[dict(path=str(p), sha256=sha(p)) for p in inputs],
                scope='Original source, release code and selected resource call edges; not guest behavior or lifetime acceptance')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
