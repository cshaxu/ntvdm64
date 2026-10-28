"""Bind original DOS directory and label owners to released DEM dispatch."""
import csv
import hashlib
import json
import re
import runpy
import struct
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit')
B = Path('build/.ninja/msvc-x86-release/worker')
S = Path('src/mvdm/dos/dem')
O = (Path('O:/repos.external/OpenNT/base/mvdm/dos/dem'),
     Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/dos/dem'))


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    h = runpy.run_path(str(A / 'host-tail-image-observer.py'))
    frozen = {r['relative_path']: r for r in csv.DictReader(
        (A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    sources = []
    for name in ('demdir.c', 'demlabel.c', 'demmsg.c'):
        row = frozen['dos/dem/' + name]
        value = sha(S / name)
        assert value == sha(O[0] / name) == sha(O[1] / name) == row['current_sha256']
        assert row['dos_catalog_rows'] == '1' and row['wow_catalog_rows'] == '2'
        sources.append(dict(relative_path=row['relative_path'], sha256=value, wow_source_ids=row['wow_source_ids']))
    graph = (B / 'build.ninja').read_text().replace('$:', ':')
    archive = next(x for x in graph.splitlines() if x.startswith('build original-mvdm-dem.lib:'))
    deps = subprocess.check_output(['ninja', '-C', str(B), '-t', 'deps'], text=True)
    for name in ('demdir', 'demlabel', 'demmsg'):
        target = 'obj/dem/' + name + '.obj'
        assert target in archive.split() and any(x.startswith('build ' + target + ':') for x in graph.splitlines())
        d = next(x for x in deps.splitlines() if x.startswith(target + ':'))
        assert '(VALID)' in d and '(STALE)' not in d
    image_path = Path('assets/release/ntvdm.exe')
    image, cached = h['Image'](image_path), h['Image'](B / 'ntvdm.exe')
    assert image.base == cached.base and image.code == cached.code
    assert all(image.data[a:b] == cached.data[a:b] for a, b, _ in image.code)
    assert sha(image_path) == sha('O:/winnt/system32/ntvdm.exe')
    syms = h['symbols']((B / 'ntvdm.exe.map').read_text())
    by_name = {
        '_demCreateDir': 'demdir', '_demDeleteDir': 'demdir', '_demQueryCurrentDir': 'demdir',
        '_demSetCurrentDir': 'demdir', '_demCreateLabel': 'demlabel', '_demDeleteLabel': 'demlabel',
        '_demCreateCommon': 'demfile', '_demFCBCommon': 'demfcb', '_demDeleteFCB': 'demfcb',
    }
    objects, symbols, bound = {}, {}, []
    for name, stem in by_name.items():
        sym = next(s for s in syms if s['name'] == name and s['owner'] == 'original-mvdm-dem:' + stem + '.obj')
        obj = h['Coff'](B / ('obj/dem/' + stem + '.obj'))
        bound.append(h['bind'](image, obj, sym))
        objects[name], symbols[name] = obj, sym
    expected = (
        ('_demCreateDir', '_CreateDirectoryOem@8'), ('_demDeleteDir', '_RemoveDirectoryOem@4'),
        ('_demSetCurrentDir', '_SetCurrentDirectoryOem@4'), ('_demSetCurrentDir', '_SetEnvironmentVariableOem@8'),
        ('_demCreateLabel', '__imp__OemToCharA@8'), ('_demCreateLabel', '__imp__SetVolumeLabelA@8'),
        ('_demDeleteLabel', '__imp__SetVolumeLabelA@8'), ('_demCreateCommon', '_demCreateLabel'),
        ('_demFCBCommon', '_demCreateLabel'), ('_demDeleteFCB', '_demDeleteLabel'),
    )
    edges = []
    for caller, target in expected:
        code, relocs = objects[caller].function(caller)
        candidates = [r for r in relocs if r[1] == target]
        assert len(candidates) == 1
        offset, _, kind = candidates[0]
        symbol = symbols[caller]
        data = h['data_at'](image, symbol['va'], len(code))
        if kind == 20:
            va = (symbol['va'] + offset + 4 + struct.unpack_from('<i', data, offset)[0]
                  - struct.unpack_from('<i', code, offset)[0]) & 0xffffffff
        else:
            assert kind == 6
            va = (struct.unpack_from('<I', data, offset)[0] - struct.unpack_from('<I', code, offset)[0]) & 0xffffffff
        found = [s for s in syms if s['name'] == target and s['va'] == va]
        assert len(found) == 1, (caller, target, hex(va))
        edges.append(dict(caller=caller, target=target, target_owner=found[0]['owner'], va=va, kind=kind))
    obj = h['Coff'](B / 'obj/dem/demdisp.obj')
    table_def = next(x for x in obj.symbols.values() if x[0] == '_apfnSVC' and x[2] > 0)
    data, rel = obj.sections[table_def[2]-1]
    table = next(x for x in syms if x['name'] == '_apfnSVC' and x['owner'] == 'original-mvdm-dem:demdisp.obj')
    initializer = (S / 'demdisp.c').read_text(encoding='latin1').split('PFNSVC  apfnSVC [] = {', 1)[1].split('};', 1)[0]
    assert not re.search(r'^\s*#', initializer, re.M)
    names = re.findall(r'^\s*(\w+)\s*,', initializer, re.M)
    entries = []
    for name, index in (('demCreate',3), ('demCreateDir',4), ('demDeleteDir',6), ('demDeleteFCB',7),
                        ('demQueryCurrentDir',19), ('demSetCurrentDir',24), ('demCreateFCB',44)):
        assert names[index] == name
        target = next(x for x in syms if x['name'] == '_' + name and x['function'])
        offset = table_def[1] + 4*index
        assert [r for r in rel if r[0] == offset] == [(offset, target['name'], 6)]
        actual = struct.unpack('<I', h['data_at'](image, table['va'] + 4*index, 4))[0]
        assert actual == (target['va'] + struct.unpack_from('<I', data, offset)[0]) & 0xffffffff
        entries.append(dict(index=index, target=target['name'], owner=target['owner'], va=actual))
    diagnostic = h['Coff'](B / 'obj/dem/demmsg.obj')
    assert not any(v[0] == '_demPrintMsg' and v[2] > 0 for v in diagnostic.symbols.values())
    assert not any(s['name'] == '_demPrintMsg' for s in syms)
    inputs = [image_path, Path('O:/winnt/system32/ntvdm.exe'), B/'ntvdm.exe', B/'ntvdm.exe.map',
              B/'build.ninja', S/'demdisp.c', S/'demfile.c', S/'demfcb.c',
              *(B/('obj/dem/'+name+'.obj') for name in ('demdir','demlabel','demmsg','demdisp','demfile','demfcb'))]
    return dict(result='pass', sources=sources, bound_functions=bound, edges=edges,
        dispatch=entries, dem_print_msg_defined=False,
        inputs=[dict(path=str(p), sha256=sha(p)) for p in inputs],
        scope='Selected image dispatch and source boundaries; no volume mutation or guest execution')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
