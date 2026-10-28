"""Bind all original 32-bit message table slots to released WOW32 pointers."""
import csv
import hashlib
import json
import runpy
import struct
from pathlib import Path

A = Path('artifacts/mvdm-audit')
B = Path('build/.ninja/msvc-x86-release/wow32')
REL = 'wow32/wmtbl32.c'


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    h = runpy.run_path(str(A / 'host-tail-image-observer.py'))
    frozen = next(x for x in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig')) if x['relative_path'] == REL)
    sources = [Path('src/mvdm') / REL, Path('O:/repos.external/OpenNT/base/mvdm') / REL,
               Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm') / REL]
    hashes = [sha(path) for path in sources]
    assert hashes == [frozen['current_sha256'], frozen['opennt_sha256'], frozen['opennt45_sha256']]
    graph = (B / 'build.ninja').read_text().replace('$:', ':')
    edge = next(x for x in graph.splitlines() if x.startswith('build obj/wmtbl32.obj:'))
    link = next(x for x in graph.splitlines() if x.startswith('build wow32.dll |'))
    assert 'obj/wmtbl32.obj' in link.split()
    image_path = Path('assets/release/WOW32.DLL')
    installed = Path('O:/winnt/system32/WOW32.DLL')
    assert sha(image_path) == sha(installed)
    image = h['Image'](image_path)
    symbols = h['symbols']((B / 'wow32.dll.map').read_text())
    obj = h['Coff'](B / 'obj/wmtbl32.obj')
    _, start, section = next(x for x in obj.symbols.values() if x[0] == '_aw32Msg' and x[2] > 0)
    body, relocations = obj.sections[section - 1]
    assert start == 0 and len(body) == 4096 and len(relocations) == 1024
    assert {p for p, _, _ in relocations} == set(range(0, 4096, 4))
    table = next(x for x in symbols if x['name'] == '_aw32Msg' and x['owner'].lower() == 'wmtbl32.obj')
    actual = h['data_at'](image, table['va'], len(body))
    slots = []
    for pos, name, kind in relocations:
        assert kind == 6
        addend = struct.unpack_from('<I', body, pos)[0]
        value = struct.unpack_from('<I', actual, pos)[0]
        candidates = [x for x in symbols if x['name'] == name]
        assert value in {(x['va'] + addend) & 0xffffffff for x in candidates}, (pos // 4, name)
        slots.append(dict(index=pos // 4, symbol=name, image_pointer=value))
    return dict(result='pass', source=REL, hashes=hashes, compile_edge=edge,
                object_sha256=sha(B / 'obj/wmtbl32.obj'), map_sha256=sha(B / 'wow32.dll.map'),
                release_sha256=sha(image_path), table=table, slots=slots,
                scope='Initial 32-to-16 message table pointers in released image, not message execution or lifetime acceptance')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
