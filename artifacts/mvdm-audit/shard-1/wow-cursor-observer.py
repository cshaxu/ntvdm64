"""Trace original WOW cursor aliases, USER registration and task retirement."""
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
O = (Path('O:/repos.external/OpenNT/base/mvdm/wow32'),
     Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/wow32'))


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    h = runpy.run_path(str(A / 'host-tail-image-observer.py'))
    frozen = {r['relative_path']: r for r in csv.DictReader(
        (A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    sources = []
    for name in ('wcurcash.c', 'wcuricon.c', 'wucursor.c'):
        row = frozen['wow32/' + name]
        digest = sha(S / name)
        assert digest == sha(O[0] / name) == sha(O[1] / name) == row['current_sha256']
        assert row['dos_catalog_rows'] == '1' and row['wow_catalog_rows'] == '2'
        sources.append(dict(relative_path=row['relative_path'], sha256=digest,
                            wow_source_ids=row['wow_source_ids']))
    recipe = (S / 'sources').read_text(encoding='latin1')
    assert sha(S / 'sources') == sha(O[0] / 'sources') == sha(O[1] / 'sources')
    for name in ('wcuricon.c', 'wucursor.c'):
        assert re.search(r'\b' + re.escape(name) + r'\b', recipe, re.I)
    assert not re.search(r'\bwcurcash\.c\b', recipe, re.I)
    graph = (B / 'build.ninja').read_text().replace('$:', ':')
    dll_link = next(x for x in graph.splitlines() if x.startswith('build wow32.dll | wow32.lib:'))
    for name in ('wcuricon', 'wucursor'):
        assert any(x.startswith('build obj/' + name + '.obj:') for x in graph.splitlines())
        assert 'obj/' + name + '.obj' in dll_link.split()
    assert 'obj/wcurcash.obj' not in dll_link.split()
    deps = subprocess.check_output(['ninja', '-C', str(B), '-t', 'deps'], text=True)
    for name in ('wcuricon', 'wucursor'):
        line = next(x for x in deps.splitlines() if x.startswith('obj/' + name + '.obj:'))
        assert '(VALID)' in line and '(STALE)' not in line
    search = subprocess.run(['rg', '-n', '-i', 'W32FindCursorIcon|W32DeleteCursorIconCashForTask|W32CheckWOWCashforIconCursors',
                             'src', '-g', '*.[ch]'], text=True, capture_output=True, check=True)
    lines = sorted(search.stdout.splitlines())
    assert len(lines) == 7 and all(re.search(r'[/\\]wcurcash\.[ch]:', x, re.I) for x in lines)
    image_path = Path('assets/release/WOW32.DLL')
    image, cached = h['Image'](image_path), h['Image'](B / 'wow32.dll')
    assert image.base == cached.base and image.code == cached.code
    assert all(image.data[a:b] == cached.data[a:b] for a, b, _ in image.code)
    assert sha(image_path) == sha('O:/winnt/system32/WOW32.DLL')
    syms = h['symbols']((B / 'wow32.dll.map').read_text())
    owners = {
        '_WU32LoadCursor@4': ('wucursor.obj', 'obj/wucursor.obj'),
        '_WU32NotifyWow@4': ('wuman.obj', 'obj/wuman.obj'),
        '_SetupResCursorIconAlias@20': ('wcuricon.obj', 'obj/wcuricon.obj'),
        '_W32CheckIfAlreadyLoaded@8': ('wcuricon.obj', 'obj/wcuricon.obj'),
        '_FreeCursorIconAlias@8': ('wcuricon.obj', 'obj/wcuricon.obj'),
        '_WowOriginalFreeTask@4': ('wkman.obj', 'obj/wkman.obj'),
        '_W32FreeTask@4': ('wow_task_cleanup_entry.obj', 'obj/wow32-dll/wow_task_cleanup_entry.obj'),
        '_UserRegisterWowHandlers@8': ('wow_user_registration_bridge.obj', 'obj/wow32-dll/wow_user_registration_bridge.obj'),
        '_wow_private_user_load_create_cursor_icon@32': ('wow_private_user_compat.obj', 'obj/wow32-dll/wow_private_user_compat.obj'),
    }
    bound, objects, symbols = {}, {}, {}
    for name, (owner, path) in owners.items():
        symbol = next(s for s in syms if s['name'] == name and s['owner'] == owner)
        obj = h['Coff'](B / path)
        bound[name] = h['bind'](image, obj, symbol)
        objects[name], symbols[name] = obj, symbol

    expected = (
        ('_WU32LoadCursor@4', '_SetupResCursorIconAlias@20'),
        ('_WU32NotifyWow@4', '_W32CheckIfAlreadyLoaded@8'),
        ('_W32FreeTask@4', '_wow_user_task_prepare_exit@0'),
        ('_W32FreeTask@4', '_WowOriginalFreeTask@4'),
        ('_WowOriginalFreeTask@4', '_FreeCursorIconAlias@8'),
        ('_UserRegisterWowHandlers@8', '_wow_private_user_load_create_cursor_icon@32'),
        ('_wow_private_user_load_create_cursor_icon@32', '__imp__CreateIconFromResourceEx@28'),
        ('_wow_private_user_load_create_cursor_icon@32', '__imp__LoadImageW@24'),
    )
    edges = []
    for caller, target in expected:
        code, relocs = objects[caller].function(caller)
        match = [r for r in relocs if r[1] == target]
        assert len(match) == 1
        offset, _, kind = match[0]
        symbol = symbols[caller]
        actual = h['data_at'](image, symbol['va'], len(code))
        if kind == 20:
            resolved = (symbol['va'] + offset + 4 + struct.unpack_from('<i', actual, offset)[0]
                        - struct.unpack_from('<i', code, offset)[0]) & 0xffffffff
        else:
            assert kind == 6
            resolved = (struct.unpack_from('<I', actual, offset)[0] - struct.unpack_from('<I', code, offset)[0]) & 0xffffffff
        matches = [s for s in syms if s['name'] == target and s['va'] == resolved]
        assert len(matches) == 1, (caller, target, hex(resolved))
        edges.append(dict(caller=caller, target=target, kind=kind, target_owner=matches[0]['owner'], va=resolved))
    headers = []
    for filename in ('wktbl2.h', 'wutbl2.h'):
        text = (S / filename).read_text(encoding='latin1')
        assert not re.search(r'^\s*#\s*(?:if|ifdef|ifndef|else|elif|endif)\b', text, re.M)
        headers.append(re.findall(r'W32FUN\(\s*([^,]+),', text))
    base = len(headers[0]) + 1
    assert base == 311
    obj = h['Coff'](B / 'obj/wowtbl.obj')
    definition = next(s for s in obj.symbols.values() if s[0] == '_aw32WOW' and s[2] > 0)
    section, relocs = obj.sections[definition[2]-1]
    table = next(s for s in syms if s['name'] == '_aw32WOW' and s['owner'] == 'wowtbl.obj')
    dispatch = []
    for ordinal, raw, label in ((173, 'WU32LoadCursor', 'LOADCURSOR'), (174, 'WU32LoadCursor', 'LOADICON'),
                                (533, 'WU32NotifyWow', 'NOTIFYWOW')):
        assert headers[1][ordinal].strip() == raw
        assert re.search(r'W32FUN\(\s*' + re.escape(raw) + r'\s*,\s*"' + label + r'"',
                         (S / 'wutbl2.h').read_text(encoding='latin1'))
        symbol = symbols['_' + raw + '@4']
        offset = definition[1] + (base + ordinal)*4
        assert [r for r in relocs if r[0] == offset] == [(offset, symbol['name'], 6)]
        value = struct.unpack('<I', h['data_at'](image, table['va'] + (base+ordinal)*4, 4))[0]
        assert value == (symbol['va'] + struct.unpack_from('<I', section, offset)[0]) & 0xffffffff
        dispatch.append(dict(ordinal=ordinal, label=label, target=symbol['name'], va=value))
    inputs = [image_path, Path('O:/winnt/system32/WOW32.DLL'), B / 'wow32.dll', B / 'wow32.dll.map',
              B / 'build.ninja', B / 'obj/wowtbl.obj', S / 'sources', S / 'wutbl2.h', S / 'wktbl2.h',
              S / 'wuman.c', S / 'wkman.c', Path('src/wow32-dll/source/wow_user_registration_bridge.c'),
              Path('src/wow32-dll/source/wow_private_user_compat.c'), Path('src/wow32-dll/source/wow_task_cleanup_entry.c')]
    inputs.extend(B / item[1] for item in owners.values())
    return dict(result='pass', sources=sources, initial_dispatch=dispatch, bound_functions=list(bound.values()),
        relocations=edges, previous_cache_references=lines,
        inputs=[dict(path=str(p), sha256=sha(p)) for p in inputs],
        scope='Image bindings and source ownership; no guest, dynamic registration, cache equivalence or lifecycle proof')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
