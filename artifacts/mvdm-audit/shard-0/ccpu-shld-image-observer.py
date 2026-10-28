"""Trace original SHLD body and decoder references into the released worker."""
import csv
import hashlib
import json
import runpy
import struct
from pathlib import Path

A = Path('artifacts/mvdm-audit'); B = Path('build/.ninja/msvc-x86-release/worker')
def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()
def inspect():
    h = runpy.run_path(str(A / 'host-tail-image-observer.py'))
    rel = 'softpc.new/base/ccpu386/shld.c'; src = Path('src/mvdm') / rel
    original = Path('O:/repos.external/OpenNT/base/mvdm') / rel
    alternate = Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm') / rel
    frozen = {r['relative_path']: r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}[rel]
    assert sha(src) == sha(original) == sha(alternate) == frozen['current_sha256'] == frozen['opennt_sha256'] == frozen['opennt45_sha256']
    graph = (B / 'build.ninja').read_text().replace('$:', ':')
    edge = next(line for line in graph.splitlines() if line.startswith('build obj/ccpu/shld.obj:'))
    archive = next(line for line in graph.splitlines() if line.startswith('build original-ccpu386.lib:'))
    assert src.resolve().as_posix().lower() in edge.lower() and 'obj/ccpu/shld.obj' in archive.split()
    image = h['Image'](Path('assets/release/ntvdm.exe')); cached = h['Image'](B / 'ntvdm.exe')
    assert image.code == cached.code and all(image.data[a:b] == cached.data[a:b] for a, b, _ in image.code)
    symbols = h['symbols']((B / 'ntvdm.exe.map').read_text())
    target = next(x for x in symbols if x['name'] == '_SHLD' and x['owner'] == 'original-ccpu386:shld.obj')
    cpu = next(x for x in symbols if x['name'] == '_ccpu' and x['owner'] == 'original-ccpu386:c_main.obj')
    body = h['bind'](image, h['Coff'](B / 'obj/ccpu/shld.obj'), target)
    obj = h['Coff'](B / 'obj/ccpu/c_main.obj'); decoder = h['bind'](image, obj, cpu)
    raw, relocations = obj.function('_ccpu'); offsets = []
    for offset, name, kind in relocations:
        if name != '_SHLD': continue
        assert kind == 6
        word = struct.unpack('<I', h['data_at'](image, cpu['va'] + offset, 4))[0]
        addend = struct.unpack_from('<I', raw, offset)[0]
        assert word == (target['va'] + addend) & 0xffffffff
        offsets.append(offset)
    assert len(offsets) == 2
    return dict(result='pass', source=rel, source_sha256=sha(src), wow_source_ids=frozen['wow_source_ids'],
                edge=edge, body=body, decoder=decoder, decoder_reference_offsets=offsets,
                image_sha256=sha('assets/release/ntvdm.exe'), map_sha256=sha(B / 'ntvdm.exe.map'),
                limits='Static selected body and references; operand, count, flags, memory and fault behavior are not proven.')
if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
