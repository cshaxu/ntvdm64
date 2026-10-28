"""Bind the original WOW SOUND thunk table and native beep boundary."""
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
M = Path('src/mvdm')
ORIGINALS = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    h = runpy.run_path(str(A / 'host-tail-image-observer.py'))
    frozen = {r['relative_path']: r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    sources = []
    for path in ('wow32/wsman.c', 'wow32/wstbl2.h'):
        f = frozen[path]; digest = sha(M / path)
        assert digest == sha(ORIGINALS[0] / path) == sha(ORIGINALS[1] / path)
        assert digest == f['current_sha256'] == f['opennt_sha256'] == f['opennt45_sha256']
        assert f['dos_catalog_rows'] == '1' and f['wow_catalog_rows'] == '2'
        sources.append(dict(relative_path=path, sha256=digest, original_path=str(ORIGINALS[0] / path),
                            dos_catalog_rows=1, wow_catalog_rows=2, wow_source_ids=f['wow_source_ids']))
    graph = (B / 'build.ninja').read_text().replace('$:', ':')
    edge = next(line for line in graph.splitlines() if line.startswith('build obj/wsman.obj:'))
    link = next(line for line in graph.splitlines() if line.startswith('build wow32.dll | wow32.lib:'))
    assert (M / 'wow32/wsman.c').resolve().as_posix().lower() in edge.lower() and 'obj/wsman.obj' in link.split()
    deps = subprocess.check_output(['ninja', '-C', str(B), '-t', 'deps'], text=True)
    blocks = deps.split('\n\n')
    dep = next(b for b in blocks if b.lower().startswith('obj/wowtbl.obj:'))
    sounddep = next(b for b in blocks if b.lower().startswith('obj/wsman.obj:'))
    assert all('(VALID)' in b.splitlines()[0] and '(STALE)' not in b for b in (dep, sounddep))
    assert 'wstbl2.h' in dep.lower()
    source_dir = M / 'wow32'
    names = []
    for filename in ('wktbl2.h', 'wutbl2.h', 'wgtbl2.h', 'wkbdtbl2.h', 'wstbl2.h'):
        text = (source_dir / filename).read_text(encoding='latin1')
        assert not re.search(r'^\s*#\s*(?:if|ifdef|ifndef|else|elif|endif)\b', text, re.M)
        names.append(re.findall(r'W32FUN\(\s*([^,]+),', text))
    assert [len(v) for v in names] == [310, 539, 491, 138, 19]
    sound_base = sum(len(v) + 1 for v in names[:-1])
    assert sound_base == 1482 and names[-1][17].strip() == 'WS32DoBeep'
    assert all(v.strip() == 'UNIMPLEMENTEDAPI' for i, v in enumerate(names[-1]) if i != 17)
    image_path = Path('assets/release/WOW32.DLL')
    image, cached = h['Image'](image_path), h['Image'](B / 'wow32.dll')
    assert image.base == cached.base and image.code == cached.code
    assert all(image.data[a:b] == cached.data[a:b] for a, b, _ in image.code)
    symbols = h['symbols']((B / 'wow32.dll.map').read_text())
    obj = h['Coff'](B / 'obj/wowtbl.obj')
    _, start, section = next(s for s in obj.symbols.values() if s[0] == '_aw32WOW' and s[2] > 0)
    body, relocs = obj.sections[section - 1]
    table = next(s for s in symbols if s['name'] == '_aw32WOW' and s['owner'] == 'wowtbl.obj')
    entries = []
    for ordinal in range(19):
        target = '_WS32DoBeep@4' if ordinal == 17 else '_WOW32UnimplementedAPI@4'
        owner = 'wsman.obj' if ordinal == 17 else 'wow32.obj'
        symbol = next(s for s in symbols if s['name'] == target and s['owner'] == owner and s['function'])
        offset = start + 4 * (sound_base + ordinal)
        assert [r for r in relocs if r[0] == offset] == [(offset, target, 6)]
        value = struct.unpack('<I', h['data_at'](image, table['va'] + 4 * (sound_base + ordinal), 4))[0]
        addend = struct.unpack_from('<I', body, offset)[0]
        assert value == (symbol['va'] + addend) & 0xffffffff
        entries.append(dict(ordinal=ordinal, target=target, owner=owner, image_pointer=value))
    beep = next(s for s in symbols if s['name'] == '_WS32DoBeep@4' and s['owner'] == 'wsman.obj')
    beep_obj = h['Coff'](B / 'obj/wsman.obj')
    beep_binding = h['bind'](image, beep_obj, beep)
    code, code_relocs = beep_obj.function(beep['name'])
    assert len(code_relocs) == 1 and code_relocs[0][1:] == ('__imp__MessageBeep@4', 6)
    offset = code_relocs[0][0]
    imported = next(s for s in symbols if s['name'] == '__imp__MessageBeep@4' and s['owner'].lower() == 'user32:user32.dll')
    actual = struct.unpack('<I', h['data_at'](image, beep['va'] + offset, 4))[0]
    addend = struct.unpack_from('<I', code, offset)[0]
    assert actual == (imported['va'] + addend) & 0xffffffff
    guest_asm, guest_def = M / 'wow16/drivers/sound/sound.asm', M / 'wow16/drivers/sound/sound.def'
    assert re.search(r'(?im)^\s*SoundThunk\s+DOBEEP\b', guest_asm.read_text())
    assert re.search(r'(?im)^\s*DOBEEP\s+@17\b', guest_def.read_text())
    inputs = [image_path, B / 'wow32.dll', B / 'wow32.dll.map', B / 'build.ninja', B / 'obj/wowtbl.obj',
              B / 'obj/wsman.obj', B / 'obj/wow32.obj', guest_asm, guest_def]
    return dict(result='pass', sources=sources, sound_base=sound_base, entries=entries,
        beep_binding=beep_binding, beep_import=dict(name=imported['name'], owner=imported['owner'], va=imported['va']),
        inputs=[dict(path=str(p), sha256=sha(p)) for p in inputs],
        scope='Initial original SOUND dispatch and native MessageBeep import; not guest or audio playback behavior')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
