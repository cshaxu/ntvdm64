"""Bind USER menu and COMM ordinal tables and selected lifecycle edges."""
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


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    h = runpy.run_path('artifacts/mvdm-audit/host-tail-image-observer.py')
    image_path = Path('assets/release/WOW32.DLL')
    image, cached = h['Image'](image_path), h['Image'](BUILD / 'wow32.dll')
    assert image.base == cached.base and image.code == cached.code
    assert all(image.data[s:e] == cached.data[s:e] for s, e, _ in image.code)
    sources = []
    for name in ('wucomm.c', 'wumenu.c', 'wowtbl.c', 'wutbl2.h', 'wktbl2.h'):
        assert sha(SRC / name) == sha(ORIG[0] / name) == sha(ORIG[1] / name)
        sources.append(dict(name=name, sha256=sha(SRC / name)))
    graph = (BUILD / 'build.ninja').read_text().replace('$:', ':')
    flags = next(line for line in graph.splitlines() if line.startswith('cflags ='))
    assert '/DPROD' in flags and not re.search(r'/D(?:DEBUG_OR_WOWPROFILE|WOWPROFILE)(?:\s|$)', flags)
    deps = subprocess.check_output(['ninja', '-C', str(BUILD), '-t', 'deps'], text=True)
    syms = h['symbols']((BUILD / 'wow32.dll.map').read_text())
    obj = h['Coff'](BUILD / 'obj/wowtbl.obj')
    _, start, section = next(s for s in obj.symbols.values() if s[0] == '_aw32WOW' and s[2] > 0)
    body, relocs = obj.sections[section - 1]
    table = next(s for s in syms if s['name'] == '_aw32WOW' and s['owner'] == 'wowtbl.obj')
    names = []
    for filename in ('wktbl2.h', 'wutbl2.h'):
        source = (SRC / filename).read_text(encoding='latin1')
        assert not re.search(r'^\s*#\s*(?:if|ifdef|ifndef|else|elif|endif)\b', source, re.M)
        names.append(re.findall(r'W32FUN\(\s*([^,]+),', source))
    base = len(names[0]) + 1
    assert base == 311
    owners = []
    for owner, expected in (('wumenu', 20), ('wucomm', 17)):
        target = f'obj/{owner}.obj'
        assert any(line.startswith('build ' + target + ':') for line in graph.splitlines())
        assert target in next(line for line in graph.splitlines() if line.startswith('build wow32.dll | wow32.lib:')).split()
        dep = next(line for line in deps.splitlines() if line.startswith(target + ':'))
        assert '(VALID)' in dep and '(STALE)' not in dep
        selected = h['Coff'](BUILD / target)
        candidates = [s for s in syms if s['owner'].lower() == owner + '.obj' and s['function']]
        entries = []
        for ordinal, raw in enumerate(names[1]):
            name = raw.strip()
            matching = [s for s in candidates if s['name'].startswith('_' + name + '@')]
            if not matching:
                continue
            assert len(matching) == 1
            symbol = matching[0]
            offset = start + (base + ordinal) * 4
            relocation = [r for r in relocs if r[0] == offset]
            assert len(relocation) == 1 and relocation[0][1] == symbol['name'] and relocation[0][2] == 6
            addend = struct.unpack_from('<I', body, offset)[0]
            value = struct.unpack('<I', h['data_at'](image, table['va'] + (base + ordinal) * 4, 4))[0]
            assert value == (symbol['va'] + addend) & 0xffffffff
            entries.append(dict(ordinal=ordinal, table_index=base + ordinal, thunk=name,
                                binding=h['bind'](image, selected, symbol)))
        assert len(entries) == expected
        owners.append(dict(owner=owner, object_sha256=sha(BUILD / target), dependency=dep, entries=entries))
    edges = []
    for source_name, source_obj, target_name, target_obj, count in (
        ('_WowOriginalFreeTask@4', 'wkman', '_FreeCommSupportResources@4', 'wucomm', 1),
        ('_WU32LoadMenu@4', 'wumenu', '_ConvertMenu16@20', 'wres16', 1),
        ('_WU32LoadMenuIndirect@4', 'wumenu', '_ConvertMenu16@20', 'wres16', 2)):
        source = next(s for s in syms if s['name'] == source_name and s['owner'] == source_obj + '.obj')
        dest = next(s for s in syms if s['name'] == target_name and s['owner'] == target_obj + '.obj')
        source_coff = h['Coff'](BUILD / ('obj/' + source_obj + '.obj'))
        bound = h['bind'](image, source_coff, source)
        code, relocations = source_coff.function(source_name)
        offsets = [p for p, n, k in relocations if n == target_name and k == 20]
        assert len(offsets) == count
        for offset in offsets:
            assert code[offset - 1] in (0xe8, 0xe9)
            displacement = struct.unpack('<i', h['data_at'](image, source['va'] + offset, 4))[0]
            addend = struct.unpack_from('<i', code, offset)[0]
            assert source['va'] + offset + 4 + displacement == dest['va'] + addend
        edges.append(dict(caller=source_name, target=target_name, offsets=offsets, binding=bound))
    cleanup = next(s for s in syms if s['name'] == '_FreeCommSupportResources@4' and s['owner'] == 'wucomm.obj')
    cleanup_bound = h['bind'](image, h['Coff'](BUILD / 'obj/wucomm.obj'), cleanup)
    return dict(result='pass', sources=sources, user_table_base=base, owners=owners, edges=edges,
        cleanup=cleanup_bound, wkman_source_sha256=sha(SRC / 'wkman.c'),
        inputs=[dict(path=str(p), sha256=sha(p)) for p in (image_path, BUILD / 'wow32.dll',
            BUILD / 'wow32.dll.map', BUILD / 'build.ninja', BUILD / 'obj/wowtbl.obj',
            BUILD / 'obj/wkman.obj', BUILD / 'obj/wres16.obj')],
        scope='Initial USER pointers, selected call relocations and masked source-object code; no guest execution or complete lifecycle')


if __name__ == '__main__':
    result = inspect()
    (OUT / 'parallel-review-fourth-evidence.json').write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
    print('pass', [(o['owner'], len(o['entries'])) for o in result['owners']], 'edges', len(result['edges']))
