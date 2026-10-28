"""Resolve selected DOS host subdispatch tables in the published worker."""
import hashlib
import json
import re
import runpy
import struct
import subprocess
from pathlib import Path

ROOT = Path('artifacts/mvdm-audit/shard-2')
BUILD = Path('build/.ninja/msvc-x86-release/worker')


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    h = runpy.run_path('artifacts/mvdm-audit/host-tail-image-observer.py')
    image = h['Image'](Path('assets/release/ntvdm.exe'))
    syms = h['symbols']((BUILD / 'ntvdm.exe.map').read_text())
    graph = (BUILD / 'build.ninja').read_text()
    deps = subprocess.check_output(['ninja', '-C', str(BUILD), '-t', 'deps'], text=True)
    specs = [('_apfnSVCCmd', 'original-mvdm-command:cmddisp.obj', 'obj/command/cmddisp.obj', 17),
             ('_apfnSVCIoctl', 'original-mvdm-dem:demioctl.obj', 'obj/dem/demioctl.obj', 19)]
    tables = []
    for name, owner, target, count in specs:
        obj = h['Coff'](BUILD / target)
        definition = [s for s in obj.symbols.values() if s[0] == name and s[2] > 0]
        assert len(definition) == 1
        _, start, section = definition[0]
        body, relocs = obj.sections[section - 1]
        table = [s for s in syms if s['name'] == name and s['owner'] == owner]
        assert len(table) == 1
        actual = h['data_at'](image, table[0]['va'], count * 4)
        selected = [(p - start, n, k) for p, n, k in relocs if start <= p < start + count * 4]
        assert len(selected) == count and {p for p, _, _ in selected} == set(range(0, count * 4, 4))
        entries = []
        for offset, symbol, kind in selected:
            assert kind == 6
            addend = struct.unpack_from('<I', body, start + offset)[0]
            value = struct.unpack_from('<I', actual, offset)[0]
            local = any(s[0] == symbol and s[2] > 0 for s in obj.symbols.values())
            matches = [s for s in syms if s['name'] == symbol and (not local or s['owner'] == owner)
                       and (s['va'] + addend) & 0xffffffff == value]
            assert matches, (name, offset, symbol, value)
            entries.append(dict(index=offset // 4, target=symbol, value=value,
                                owners=sorted({s['owner'] for s in matches})))
        entry = next(line for line in graph.splitlines() if line.startswith('build ' + target + ':'))
        dep = next(line for line in deps.splitlines() if line.startswith(target + ':'))
        assert '(VALID)' in dep and '(STALE)' not in dep
        tables.append(dict(name=name, owner=owner, object=target, object_sha256=sha(BUILD / target),
                           build_edge=entry, dependency=dep, entries=entries))
    duplicate = Path('src/mvdm/dos/command/cmdpif.h')
    selected = Path('src/mvdm/softpc.new/host/inc/nt_pif.h')
    old = duplicate.read_text(encoding='latin1')
    active = selected.read_text(encoding='latin1')
    assert 'This structure is copied from NT_PIF.H' in old
    assert re.search(r'\bchar\s+fgprio\s*;', old) and not re.search(r'\bchar\s+fgprio\s*;', active)
    matches = subprocess.run(['rg', '-n', '-i', r'^\s*#\s*include\s*[<"][^">]*cmdpif\.h[">]',
                              'src', '-g', '*.[ch]'], text=True, capture_output=True)
    assert matches.returncode == 1 and not matches.stdout
    blocks = [b for b in re.split(r'(?m)(?=^obj/)', deps) if b.strip()]
    assert blocks and all('(VALID)' in b.splitlines()[0] and '(STALE)' not in b for b in blocks)
    assert not any('/dos/command/cmdpif.h' in b.lower().replace('\\', '/') for b in blocks)
    assert any('/softpc.new/host/inc/nt_pif.h' in b.lower().replace('\\', '/') for b in blocks)
    providers = []
    for name, owner, objname in [('_GetPIFData', 'original-softpc-host-roots:nt_pif.obj', 'obj/host/nt_pif.obj'),
                                  ('_cmdCheckForPIF', 'original-mvdm-command:cmdpif.obj', 'obj/command/cmdpif.obj')]:
        symbol = next(s for s in syms if s['name'] == name and s['owner'] == owner and s['function'])
        item = h['bind'](image, h['Coff'](BUILD / objname), symbol)
        item['object_sha256'] = sha(BUILD / objname)
        providers.append(item)
    return dict(result='pass', tables=tables,
                pif=dict(duplicate_sha256=sha(duplicate), selected_header_sha256=sha(selected),
                    direct_include_matches=0, dependency_blocks=len(blocks), providers=providers),
                image_sha256=sha('assets/release/ntvdm.exe'),
                map_sha256=sha(BUILD / 'ntvdm.exe.map'), graph_sha256=sha(BUILD / 'build.ninja'),
                scope='Initial image dispatch pointers only; no guest execution, failure or resource lifecycle claim')


if __name__ == '__main__':
    result = inspect()
    (ROOT / 'parallel-review-next-evidence.json').write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
    print('pass', [(t['name'], len(t['entries'])) for t in result['tables']])
