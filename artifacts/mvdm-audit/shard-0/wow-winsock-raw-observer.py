"""Bind original synchronous Winsock slots to the released WOW32 image."""
import csv
import hashlib
import json
import re
import runpy
import struct
from pathlib import Path

A = Path('artifacts/mvdm-audit')
B = Path('build/.ninja/msvc-x86-release/wow32')
S = Path('src/mvdm/wow32')
P = 'wow32/wsraw.c'


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    h = runpy.run_path(str(A / 'host-tail-image-observer.py'))
    frozen = {r['relative_path']: r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    paths = [Path('src/mvdm') / P, Path('O:/repos.external/OpenNT/base/mvdm') / P,
             Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm') / P]
    row = frozen[P]
    assert sha(paths[0]) == sha(paths[1]) == sha(paths[2]) == row['current_sha256'] == row['opennt_sha256'] == row['opennt45_sha256']
    graph = (B / 'build.ninja').read_text().replace('$:', ':')
    edge = next(x for x in graph.splitlines() if x.startswith('build obj/wsraw.obj:'))
    link = next(x for x in graph.splitlines() if x.startswith('build wow32.dll | wow32.lib:'))
    assert paths[0].resolve().as_posix().lower() in edge.lower() and 'obj/wsraw.obj' in link.split()
    imagepath = Path('assets/release/WOW32.DLL')
    image, cached = h['Image'](imagepath), h['Image'](B / 'wow32.dll')
    assert image.base == cached.base and image.code == cached.code
    assert all(image.data[a:b] == cached.data[a:b] for a, b, _ in image.code)
    symbols = h['symbols']((B / 'wow32.dll.map').read_text())
    tableobj, owner = h['Coff'](B / 'obj/wowtbl.obj'), h['Coff'](B / 'obj/wsraw.obj')
    definition = [s for s in tableobj.symbols.values() if s[0] == '_aw32WOW' and s[2] > 0]
    assert len(definition) == 1
    _, start, section = definition[0]
    body, relocs = tableobj.sections[section - 1]
    table = next(s for s in symbols if s['name'] == '_aw32WOW' and s['owner'] == 'wowtbl.obj')
    headers = ('wktbl2.h', 'wutbl2.h', 'wgtbl2.h', 'wkbdtbl2.h', 'wstbl2.h', 'wshtbl2.h', 'wwstbl2.h')
    names = []
    for name in headers:
        assert sha(S / name) == sha(Path('O:/repos.external/OpenNT/base/mvdm/wow32') / name) == sha(Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/wow32') / name)
        text = (S / name).read_text(encoding='latin1')
        assert not re.search(r'^\s*#\s*(?:if|ifdef|ifndef|else|elif|endif)\b', text, re.M)
        names.append(re.findall(r'W32FUN\(\s*([^,]+),', text))
    base = sum(len(x) for x in names[:-1]) + len(names) - 1
    assert base == 1607
    entries = []
    for ordinal, raw in enumerate(names[-1]):
        name = raw.strip()
        candidates = [s for s in symbols if s['owner'].lower() == 'wsraw.obj' and s['function'] and s['name'].startswith('_'+name+'@')]
        if not candidates:
            continue
        assert len(candidates) == 1
        sym = candidates[0]
        offset = start + (base + ordinal) * 4
        rel = [r for r in relocs if r[0] == offset]
        assert len(rel) == 1 and rel[0][1] == sym['name'] and rel[0][2] == 6
        addend = struct.unpack_from('<I', body, offset)[0]
        value = struct.unpack('<I', h['data_at'](image, table['va'] + (base + ordinal) * 4, 4))[0]
        assert value == (sym['va'] + addend) & 0xffffffff
        entries.append(dict(ordinal=ordinal, thunk=name, binding=h['bind'](image, owner, sym)))
    assert [x['ordinal'] for x in entries] == list(range(1, 24))
    inputs = [*paths, B/'build.ninja', B/'obj/wsraw.obj', B/'obj/wowtbl.obj', B/'wow32.dll.map', imagepath, *[S/n for n in headers]]
    return dict(result='pass', source=P, source_sha256=sha(paths[0]), table_base=base, entries=entries,
                inputs=[dict(path=str(p), sha256=sha(p)) for p in inputs],
                scope='Initial synchronous Winsock bindings only; no network, concurrent task or guest behavior acceptance')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
