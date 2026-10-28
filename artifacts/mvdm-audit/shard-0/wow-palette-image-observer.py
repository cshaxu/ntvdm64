"""Bind original palette GDI and USER entries to the published WOW32 image."""
import json
import re
import runpy
import struct
from pathlib import Path

A = Path('artifacts/mvdm-audit')
B = Path('build/.ninja/msvc-x86-release/wow32')


def inspect():
    gdi = runpy.run_path(str(A / 'shard-2/parallel-review-third-observer.py'))['inspect'](('wgpal',))
    h = runpy.run_path(str(A / 'host-tail-image-observer.py'))
    image = h['Image'](Path('assets/release/WOW32.DLL'))
    symbols = h['symbols']((B / 'wow32.dll.map').read_text())
    table_obj, owner = h['Coff'](B / 'obj/wowtbl.obj'), h['Coff'](B / 'obj/wgpal.obj')
    definition = [s for s in table_obj.symbols.values() if s[0] == '_aw32WOW' and s[2] > 0]
    assert len(definition) == 1
    _, start, section = definition[0]
    body, relocs = table_obj.sections[section - 1]
    table = next(s for s in symbols if s['name'] == '_aw32WOW' and s['owner'] == 'wowtbl.obj')
    root = Path('src/mvdm/wow32')
    kernel = re.findall(r'W32FUN\(\s*([^,]+),', (root / 'wktbl2.h').read_text(encoding='latin1'))
    user = re.findall(r'W32FUN\(\s*([^,]+),', (root / 'wutbl2.h').read_text(encoding='latin1'))
    base = len(kernel) + 1
    assert base == 311
    entries = []
    for name in ('WU32SelectPalette', 'WU32RealizePalette'):
        positions = [i for i, raw in enumerate(user) if raw.strip() == name]
        assert len(positions) == 1
        ordinal = positions[0]
        sym = next(s for s in symbols if s['name'] == '_' + name + '@4' and s['owner'] == 'wgpal.obj')
        offset = start + (base + ordinal) * 4
        relocation = [r for r in relocs if r[0] == offset]
        assert len(relocation) == 1 and relocation[0][1] == sym['name'] and relocation[0][2] == 6
        addend = struct.unpack_from('<I', body, offset)[0]
        value = struct.unpack('<I', h['data_at'](image, table['va'] + (base + ordinal) * 4, 4))[0]
        assert value == (sym['va'] + addend) & 0xffffffff
        entries.append(dict(ordinal=ordinal, thunk=name, binding=h['bind'](image, owner, sym)))
    return dict(result='pass', gdi=gdi, user_table_base=base, user_entries=entries,
                scope='Original source and initial GDI/USER table bindings; not guest behavior acceptance')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
