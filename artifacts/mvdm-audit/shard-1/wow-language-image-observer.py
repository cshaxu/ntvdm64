"""Bind original WOW language thunks to initial USER table and host imports."""
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
REL = 'wow32/wulang.c'
NAMES = ('WU32AnsiLower', 'WU32AnsiLowerBuff', 'WU32AnsiNext', 'WU32AnsiPrev',
         'WU32AnsiUpper', 'WU32AnsiUpperBuff', 'WU32IsCharAlpha', 'WU32IsCharAlphaNumeric',
         'WU32IsCharLower', 'WU32IsCharUpper', 'WU32lstrcmp', 'WU32lstrcmpi')


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    h = runpy.run_path(str(A / 'host-tail-image-observer.py'))
    with (A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig', newline='') as stream:
        f = next(r for r in csv.DictReader(stream) if r['relative_path'] == REL)
    source = Path('src/mvdm') / REL
    original = Path('O:/repos.external/OpenNT/base/mvdm') / REL
    second = Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm') / REL
    assert sha(source) == sha(original) == sha(second) == f['current_sha256'] == f['opennt_sha256'] == f['opennt45_sha256']
    assert f['dos_catalog_rows'] == '1' and f['wow_catalog_rows'] == '2'
    graph = (B / 'build.ninja').read_text().replace('$:', ':')
    target = 'obj/wulang.obj'
    edge = next(line for line in graph.splitlines() if line.startswith('build ' + target + ':'))
    link = next(line for line in graph.splitlines() if line.startswith('build wow32.dll | wow32.lib:'))
    assert source.resolve().as_posix().lower() in edge.lower() and target in link.split()
    deps = subprocess.check_output(['ninja', '-C', str(B), '-t', 'deps'], text=True)
    dep = next(line for line in deps.splitlines() if line.startswith(target + ':'))
    assert '(VALID)' in dep and '(STALE)' not in dep
    image_path = Path('assets/release/WOW32.DLL')
    image, cached = h['Image'](image_path), h['Image'](B / 'wow32.dll')
    assert image.base == cached.base and image.code == cached.code
    assert all(image.data[a:b] == cached.data[a:b] for a, b, _ in image.code)
    symbols = h['symbols']((B / 'wow32.dll.map').read_text())
    obj = h['Coff'](B / target)
    functions, imports = [], []
    for name in NAMES:
        symbol = next(s for s in symbols if s['name'] == '_' + name + '@4' and s['owner'] == 'wulang.obj' and s['function'])
        functions.append(h['bind'](image, obj, symbol))
        code, relocs = obj.function(symbol['name'])
        selected = [(off, target_name, kind) for off, target_name, kind in relocs if target_name.startswith('__imp__')]
        assert len(selected) == 1 and selected[0][2] == 6
        off, target_name, _ = selected[0]
        address = struct.unpack('<I', h['data_at'](image, symbol['va'] + off, 4))[0]
        addend = struct.unpack_from('<I', code, off)[0]
        target_symbol = next(s for s in symbols if s['name'] == target_name and s['va'] == (address - addend) & 0xffffffff)
        assert target_symbol['owner'].lower().endswith(':user32.dll') or target_symbol['owner'].lower().endswith(':kernel32.dll')
        imports.append(dict(caller=symbol['name'], target=target_name, owner=target_symbol['owner'], va=target_symbol['va']))
    kernel = Path('src/mvdm/wow32/wktbl2.h').read_text(encoding='latin1')
    user = Path('src/mvdm/wow32/wutbl2.h').read_text(encoding='latin1')
    assert all(not re.search(r'^\s*#\s*(?:if|ifdef|ifndef|else|elif|endif)\b', t, re.M) for t in (kernel, user))
    kernel_names = re.findall(r'W32FUN\(\s*([^,]+),', kernel)
    user_names = re.findall(r'W32FUN\(\s*([^,]+),', user)
    base = len(kernel_names) + 1
    assert base == 311 and len(user_names) == 539
    tab = h['Coff'](B / 'obj/wowtbl.obj')
    _, start, section = next(s for s in tab.symbols.values() if s[0] == '_aw32WOW' and s[2] > 0)
    body, relocs = tab.sections[section - 1]
    table = next(s for s in symbols if s['name'] == '_aw32WOW' and s['owner'] == 'wowtbl.obj')
    entries = []
    for ordinal, name in enumerate(user_names):
        name = name.strip()
        if name not in NAMES:
            continue
        symbol = next(s for s in symbols if s['name'] == '_' + name + '@4' and s['owner'] == 'wulang.obj')
        off = start + (base + ordinal) * 4
        assert [r for r in relocs if r[0] == off] == [(off, symbol['name'], 6)]
        actual = struct.unpack('<I', h['data_at'](image, table['va'] + (base + ordinal) * 4, 4))[0]
        assert actual == (symbol['va'] + struct.unpack_from('<I', body, off)[0]) & 0xffffffff
        entries.append(dict(ordinal=ordinal, target=symbol['name'], va=actual))
    assert len(entries) == len(NAMES) and len({e['target'] for e in entries}) == len(NAMES)
    inputs = (source, original, second, image_path, B / 'wow32.dll', B / 'wow32.dll.map', B / 'build.ninja',
              B / target, B / 'obj/wowtbl.obj', Path('src/mvdm/wow32/wktbl2.h'), Path('src/mvdm/wow32/wutbl2.h'))
    return dict(result='pass', relative_path=REL, sha256=sha(source), wow_source_ids=f['wow_source_ids'],
        edge=edge, dependency=dep, functions=functions, user_table_base=base, entries=entries, imports=imports,
        inputs=[dict(path=str(p), sha256=sha(p)) for p in inputs],
        scope='Initial original USER table, selected code and native imports; no runtime locale or guest behavior claim')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
