"""Bind original WOW spool declarations and six GDI job thunks."""
import csv
import hashlib
import json
import re
import runpy
import struct
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit')
B = Path('build/.ninja/msvc-x86-release/wow32')
M = Path('src/mvdm')
ORIGINALS = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    h = runpy.run_path(str(A / 'host-tail-image-observer.py'))
    frozen = {r['relative_path']: r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    sources = []
    for path in ('wow32/wspool.h', 'wow32/winspldl.h', 'wow32/wspool.c'):
        f = frozen[path]; digest = sha(M / path)
        assert digest == sha(ORIGINALS[0] / path) == sha(ORIGINALS[1] / path)
        assert digest == f['current_sha256'] == f['opennt_sha256'] == f['opennt45_sha256']
        assert f['dos_catalog_rows'] == '1' and f['wow_catalog_rows'] == '2'
        sources.append(dict(relative_path=path, sha256=digest, original_path=str(ORIGINALS[0] / path),
                            dos_catalog_rows=1, wow_catalog_rows=2, wow_source_ids=f['wow_source_ids']))
    graph = (B / 'build.ninja').read_text().replace('$:', ':')
    edge = next(s for s in graph.splitlines() if s.startswith('build obj/wspool.obj:'))
    link = next(s for s in graph.splitlines() if s.startswith('build wow32.dll | wow32.lib:'))
    assert (M / 'wow32/wspool.c').resolve().as_posix().lower() in edge.lower() and 'obj/wspool.obj' in link.split()
    deps = subprocess.check_output(['ninja', '-C', str(B), '-t', 'deps'], text=True)
    blocks = deps.split('\n\n')
    observed = []
    for target in ('obj/wspool.obj', 'obj/wowtbl.obj'):
        block = next(b for b in blocks if b.lower().startswith(target + ':'))
        assert '(VALID)' in block.splitlines()[0] and '(STALE)' not in block and 'wspool.h' in block.lower()
        observed.append(block.splitlines()[0])
    winspl_users = [b.splitlines()[0] for b in blocks if 'winspldl.h' in b.lower()]
    image_path = Path('assets/release/WOW32.DLL')
    image, cached = h['Image'](image_path), h['Image'](B / 'wow32.dll')
    assert image.base == cached.base and image.code == cached.code
    assert all(image.data[a:b] == cached.data[a:b] for a, b, _ in image.code)
    syms = h['symbols']((B / 'wow32.dll.map').read_text())
    table_obj, owner_obj = h['Coff'](B / 'obj/wowtbl.obj'), h['Coff'](B / 'obj/wspool.obj')
    _, start, section = next(s for s in table_obj.symbols.values() if s[0] == '_aw32WOW' and s[2] > 0)
    body, relocs = table_obj.sections[section - 1]
    table = next(s for s in syms if s['name'] == '_aw32WOW' and s['owner'] == 'wowtbl.obj')
    groups = []
    for name in ('wktbl2.h', 'wutbl2.h', 'wgtbl2.h'):
        text = (M / 'wow32' / name).read_text(encoding='latin1')
        assert not re.search(r'^\s*#\s*(?:if|ifdef|ifndef|else|elif|endif)\b', text, re.M)
        groups.append(re.findall(r'W32FUN\(\s*([^,]+),', text))
    base = len(groups[0]) + len(groups[1]) + 2
    assert base == 851
    wanted = {'WG32OpenJob', 'WG32WriteSpool', 'WG32CloseJob', 'WG32DeleteJob', 'WG32StartSpoolPage', 'WG32EndSpoolPage'}
    entries = []
    for ordinal, name in enumerate(groups[2]):
        name = name.strip()
        if name not in wanted:
            continue
        symbol = next(s for s in syms if s['name'] == '_' + name + '@4' and s['owner'].lower() == 'wspool.obj')
        offset = start + 4 * (base + ordinal)
        assert [r for r in relocs if r[0] == offset] == [(offset, symbol['name'], 6)]
        value = struct.unpack('<I', h['data_at'](image, table['va'] + 4 * (base + ordinal), 4))[0]
        assert value == (symbol['va'] + struct.unpack_from('<I', body, offset)[0]) & 0xffffffff
        entries.append(dict(ordinal=ordinal, target=symbol['name'], binding=h['bind'](image, owner_obj, symbol)))
    assert len(entries) == len(wanted) and {e['target'][1:-2] for e in entries} == wanted
    call = next(s for s in syms if s['name'] == '_WG32OpenJob@4' and s['owner'].lower() == 'wspool.obj')
    loader = next(s for s in syms if s['name'] == '_LoadLibraryAndGetProcAddresses@12' and s['owner'].lower() == 'wgprnset.obj')
    code, rel = owner_obj.function(call['name'])
    offsets = [p for p, n, k in rel if n == loader['name'] and k == 20]
    assert len(offsets) == 2
    for offset in offsets:
        assert code[offset - 1] in (0xe8, 0xe9)
        displacement = struct.unpack('<i', h['data_at'](image, call['va'] + offset, 4))[0]
        assert call['va'] + offset + 4 + displacement == loader['va'] + struct.unpack_from('<i', code, offset)[0]
    files = [image_path, B / 'wow32.dll', B / 'wow32.dll.map', B / 'build.ninja'] + [B / ('obj/' + s + '.obj') for s in ('wspool', 'wowtbl', 'wgprnset')]
    return dict(result='pass', sources=sources, dependency_headers=observed, winspldl_dependent_objects=winspl_users,
        gdi_base=base, entries=entries, loader_call_offsets=offsets,
        inputs=[dict(path=str(p), sha256=sha(p)) for p in files],
        scope='Original declarations, initial GDI table and selected loading calls; not printer, guest or teardown behavior')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
