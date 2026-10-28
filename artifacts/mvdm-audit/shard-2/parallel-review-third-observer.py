"""Bind selected original GDI thunk ordinals to the released WOW image."""
import hashlib
import json
import re
import runpy
import struct
import subprocess
from pathlib import Path

OUT = Path('artifacts/mvdm-audit/shard-2')
BUILD = Path('build/.ninja/msvc-x86-release/wow32')
SRC = Path('src/mvdm/wow32')
ORIG = (Path('O:/repos.external/OpenNT/base/mvdm/wow32'),
        Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/wow32'))
OWNERS = ('wgfont', 'wgtext', 'wgmeta')


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect(owners=OWNERS):
    h = runpy.run_path('artifacts/mvdm-audit/host-tail-image-observer.py')
    image_path = Path('assets/release/WOW32.DLL')
    image, cached = h['Image'](image_path), h['Image'](BUILD / 'wow32.dll')
    assert image.base == cached.base and image.code == cached.code
    assert all(image.data[s:e] == cached.data[s:e] for s, e, _ in image.code)
    names = tuple(owner + '.c' for owner in owners) + ('wowtbl.c', 'wgtbl2.h', 'wutbl2.h', 'wktbl2.h')
    sources = []
    for name in names:
        assert sha(SRC / name) == sha(ORIG[0] / name) == sha(ORIG[1] / name)
        sources.append(dict(name=name, sha256=sha(SRC / name)))
    graph = (BUILD / 'build.ninja').read_text().replace('$:', ':')
    flags = next(line for line in graph.splitlines() if line.startswith('cflags ='))
    assert '/DPROD' in flags and not re.search(r'/D(?:DEBUG_OR_WOWPROFILE|WOWPROFILE)(?:\s|$)', flags)
    deps = subprocess.check_output(['ninja', '-C', str(BUILD), '-t', 'deps'], text=True)
    syms = h['symbols']((BUILD / 'wow32.dll.map').read_text())
    obj = h['Coff'](BUILD / 'obj/wowtbl.obj')
    definition = [s for s in obj.symbols.values() if s[0] == '_aw32WOW' and s[2] > 0]
    assert len(definition) == 1
    _, start, section = definition[0]
    body, relocs = obj.sections[section - 1]
    table = next(s for s in syms if s['name'] == '_aw32WOW' and s['owner'] == 'wowtbl.obj')
    table_names = []
    for name in ('wktbl2.h', 'wutbl2.h', 'wgtbl2.h'):
        text = (SRC / name).read_text(encoding='latin1')
        assert not re.search(r'^\s*#\s*(?:if|ifdef|ifndef|else|elif|endif)\b', text, re.M)
        table_names.append(re.findall(r'W32FUN\(\s*([^,]+),', text))
    base = len(table_names[0]) + len(table_names[1]) + 2
    assert base == 851
    output = []
    for owner in owners:
        target = f'obj/{owner}.obj'
        assert any(line.startswith('build ' + target + ':') for line in graph.splitlines())
        assert target in next(line for line in graph.splitlines() if line.startswith('build wow32.dll | wow32.lib:')).split()
        dep = next(line for line in deps.splitlines() if line.startswith(target + ':'))
        assert '(VALID)' in dep and '(STALE)' not in dep
        owner_obj = h['Coff'](BUILD / target)
        owner_functions = [s for s in syms if s['owner'].lower() == owner + '.obj' and s['function']]
        entries = []
        for ordinal, raw in enumerate(table_names[2]):
            name = raw.strip()
            matches = [s for s in owner_functions if s['name'].startswith('_' + name + '@')]
            if not matches:
                continue
            assert len(matches) == 1
            symbol = matches[0]
            offset = start + (base + ordinal) * 4
            relocation = [r for r in relocs if r[0] == offset]
            assert len(relocation) == 1 and relocation[0][1] == symbol['name'] and relocation[0][2] == 6
            addend = struct.unpack_from('<I', body, offset)[0]
            value = struct.unpack('<I', h['data_at'](image, table['va'] + (base + ordinal) * 4, 4))[0]
            assert value == (symbol['va'] + addend) & 0xffffffff
            entries.append(dict(ordinal=ordinal, table_index=base + ordinal, thunk=name,
                                binding=h['bind'](image, owner_obj, symbol)))
        assert len(entries) == {'wgfont': 8, 'wgtext': 12, 'wgmeta': 8, 'wgpal': 10, 'wgdi': 119}[owner]
        output.append(dict(owner=owner, object_sha256=sha(BUILD / target), dependency=dep, entries=entries))
    return dict(result='pass', sources=sources, gdi_table_base=base, owners=output,
        inputs=[dict(path=str(p), sha256=sha(p)) for p in (image_path, BUILD / 'wow32.dll',
            BUILD / 'wow32.dll.map', BUILD / 'build.ninja', BUILD / 'obj/wowtbl.obj')],
        scope='Original source, linked initial GDI pointers and masked code bytes; no guest dispatch or callback execution claim')


if __name__ == '__main__':
    x = inspect()
    (OUT / 'parallel-review-third-evidence.json').write_text(json.dumps(x, indent=2) + '\n', encoding='utf-8')
    print('pass', [(v['owner'], len(v['entries'])) for v in x['owners']])
