"""Bind original WOW tabbed-text thunks and their initial USER dispatch."""
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
S = Path('src/mvdm/wow32')
PRIMARY = Path('O:/repos.external/OpenNT/base/mvdm/wow32')
SECONDARY = Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/wow32')


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def helper(text):
    start = text.index('VOID getintarray16(')
    return text[start:text.index('\n}', start) + 2]


def inspect():
    h = runpy.run_path(str(A / 'host-tail-image-observer.py'))
    frozen = {r['relative_path']: r for r in csv.DictReader(
        (A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    row = frozen['wow32/wutext.c']
    assert sha(S / 'wutext.c') == sha(PRIMARY / 'wutext.c') == sha(SECONDARY / 'wutext.c') == row['current_sha256']
    assert row['dos_catalog_rows'] == '1' and row['wow_catalog_rows'] == '2'
    for name in ('wutbl2.h', 'wktbl2.h', 'wowtbl.c'):
        assert sha(S / name) == sha(PRIMARY / name) == sha(SECONDARY / name)
    source_helper = helper((S / 'wstruc.c').read_text(encoding='latin1'))
    assert source_helper == helper((PRIMARY / 'wstruc.c').read_text(encoding='latin1'))
    image_path = Path('assets/release/WOW32.DLL')
    image, cached = h['Image'](image_path), h['Image'](B / 'wow32.dll')
    assert image.base == cached.base and image.code == cached.code
    assert all(image.data[s:e] == cached.data[s:e] for s, e, _ in image.code)
    assert sha(image_path) == sha('O:/winnt/system32/WOW32.DLL')
    graph = (B / 'build.ninja').read_text().replace('$:', ':')
    assert any(line.startswith('build obj/wutext.obj:') for line in graph.splitlines())
    assert 'obj/wutext.obj' in next(line for line in graph.splitlines() if line.startswith('build wow32.dll | wow32.lib:')).split()
    deps = subprocess.check_output(['ninja', '-C', str(B), '-t', 'deps'], text=True)
    dep = next(line for line in deps.splitlines() if line.startswith('obj/wutext.obj:'))
    assert '(VALID)' in dep and '(STALE)' not in dep
    flags = next(line for line in graph.splitlines() if line.startswith('cflags ='))
    assert '/DPROD' in flags and not re.search(r'/D(?:DEBUG_OR_WOWPROFILE|WOWPROFILE)(?:\s|$)', flags)
    syms = h['symbols']((B / 'wow32.dll.map').read_text())
    obj, table_obj = h['Coff'](B / 'obj/wutext.obj'), h['Coff'](B / 'obj/wowtbl.obj')
    definition = next(s for s in table_obj.symbols.values() if s[0] == '_aw32WOW' and s[2] > 0)
    body, relocs = table_obj.sections[definition[2]-1]
    table = next(s for s in syms if s['name'] == '_aw32WOW' and s['owner'] == 'wowtbl.obj')
    entries = []
    headers = []
    for name in ('wktbl2.h', 'wutbl2.h'):
        text = (S / name).read_text(encoding='latin1')
        assert not re.search(r'^\s*#\s*(?:if|ifdef|ifndef|else|elif|endif)\b', text, re.M)
        headers.append(re.findall(r'W32FUN\(\s*([^,]+),', text))
    base = len(headers[0]) + 1
    assert base == 311
    for raw, expected_ordinal, native in (('WU32TabbedTextOut', 196, '__imp__TabbedTextOutA@32'),
                                          ('WU32GetTabbedTextExtent', 197, '__imp__GetTabbedTextExtentA@20')):
        assert headers[1][expected_ordinal].strip() == raw
        symbol = next(s for s in syms if s['name'] == '_' + raw + '@4' and s['owner'] == 'wutext.obj')
        offset = definition[1] + (base + expected_ordinal) * 4
        assert [r for r in relocs if r[0] == offset] == [(offset, symbol['name'], 6)]
        value = struct.unpack('<I', h['data_at'](image, table['va'] + (base + expected_ordinal) * 4, 4))[0]
        assert value == (symbol['va'] + struct.unpack_from('<I', body, offset)[0]) & 0xffffffff
        objbody, objrel = obj.function(symbol['name'])
        actual = h['data_at'](image, symbol['va'], len(objbody))
        binding = h['bind'](image, obj, symbol)
        call_edges = []
        for displacement, target, kind in objrel:
            candidates = [s for s in syms if s['name'] == target]
            if kind == 20:
                v = (symbol['va'] + displacement + 4 + struct.unpack_from('<i', actual, displacement)[0]
                     - struct.unpack_from('<i', objbody, displacement)[0]) & 0xffffffff
            elif kind == 6:
                v = (struct.unpack_from('<I', actual, displacement)[0] - struct.unpack_from('<I', objbody, displacement)[0]) & 0xffffffff
            else:
                raise AssertionError((target, kind))
            found = [s for s in candidates if s['va'] == v]
            assert len(found) == 1, (target, hex(v))
            call_edges.append(dict(offset=displacement, target=target, owner=found[0]['owner'], va=v, kind=kind))
        assert any(e['target'] == '_getintarray16@12' and e['owner'] == 'wstruc.obj' for e in call_edges)
        assert any(e['target'] == native and e['owner'].lower() == 'user32:user32.dll' for e in call_edges)
        entries.append(dict(ordinal=expected_ordinal, table_index=base+expected_ordinal, binding=binding, edges=call_edges))
    inputs = [image_path, Path('O:/winnt/system32/WOW32.DLL'), B / 'wow32.dll', B / 'wow32.dll.map',
              B / 'build.ninja', B / 'obj/wowtbl.obj', B / 'obj/wutext.obj', S / 'wutext.c', S / 'wstruc.c',
              S / 'wutbl2.h', S / 'wktbl2.h', S / 'wowtbl.c']
    return dict(result='pass', source=row['relative_path'], source_sha256=row['current_sha256'],
        wow_source_ids=row['wow_source_ids'], object_dependency=dep, user_table_base=base, entries=entries,
        getintarray16_source_sha256=hashlib.sha256(source_helper.encode('latin1')).hexdigest().upper(),
        inputs=[dict(path=str(p), sha256=sha(p)) for p in inputs],
        scope='Initial USER dispatch and masked object code/call bindings; not guest execution or failure-path acceptance')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
