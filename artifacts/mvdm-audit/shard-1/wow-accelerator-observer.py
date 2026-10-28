"""Read-only binding of the original accelerator owner in released WOW32."""
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
REL = 'wow32/waccel.c'


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    h = runpy.run_path(str(A / 'host-tail-image-observer.py'))
    with (A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig', newline='') as stream:
        f = next(row for row in csv.DictReader(stream) if row['relative_path'] == REL)
    source = Path('src/mvdm') / REL
    original = Path('O:/repos.external/OpenNT/base/mvdm') / REL
    original45 = Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm') / REL
    assert sha(source) == sha(original) == sha(original45) == f['current_sha256'] == f['opennt_sha256'] == f['opennt45_sha256']
    assert f['dos_catalog_rows'] == '1' and f['wow_catalog_rows'] == '2'
    graph = (B / 'build.ninja').read_text().replace('$:', ':')
    target = 'obj/waccel.obj'
    edge = next(line for line in graph.splitlines() if line.startswith('build ' + target + ':'))
    link = next(line for line in graph.splitlines() if line.startswith('build wow32.dll | wow32.lib:'))
    assert source.resolve().as_posix().lower() in edge.lower() and target in link.split()
    deps = subprocess.check_output(['ninja', '-C', str(B), '-t', 'deps'], text=True)
    dependency = next(line for line in deps.splitlines() if line.startswith(target + ':'))
    assert '(VALID)' in dependency and '(STALE)' not in dependency
    release = Path('assets/release/WOW32.DLL')
    image, cached = h['Image'](release), h['Image'](B / 'wow32.dll')
    assert image.base == cached.base and image.code == cached.code
    assert all(image.data[a:b] == cached.data[a:b] for a, b, _ in image.code)
    symbols = h['symbols']((B / 'wow32.dll.map').read_text())
    obj = h['Coff'](B / target)
    selected = [s for s in symbols if s['owner'] == 'waccel.obj' and s['function']]
    assert len(selected) == 8
    functions = [h['bind'](image, obj, s) for s in selected]
    calls = []
    for caller, owner, object_path, callee in (
        ('_WU32NotifyWow@4', 'wuman.obj', 'obj/wuman.obj', '_WU32LoadAccelerators@4'),
        ('_WowOriginalFreeTask@4', 'wkman.obj', 'obj/wkman.obj', '_DestroyAccelAlias@4'),
        ('_WU32LoadAccelerators@4', 'waccel.obj', target, '_CreateAccel32@8'),
        ('_WU32LoadAccelerators@4', 'waccel.obj', target, '_SetupAccelAlias@16'),
        ('_SetupAccelAlias@16', 'waccel.obj', target, '_malloc_w_small@4'),
    ):
        caller_symbol = next(s for s in symbols if s['name'] == caller and s['owner'] == owner and s['function'])
        callee_symbol = next(s for s in symbols if s['name'] == callee and s['function'])
        caller_obj = h['Coff'](B / object_path)
        body, relocs = caller_obj.function(caller)
        edges = [(p, kind) for p, name, kind in relocs if name == callee]
        assert len(edges) == 1 and edges[0][1] == 20
        p = edges[0][0]
        actual = h['data_at'](image, caller_symbol['va'], len(body))
        address = (caller_symbol['va'] + p + 4 + struct.unpack_from('<i', actual, p)[0]
                   - struct.unpack_from('<i', body, p)[0]) & 0xffffffff
        assert address == callee_symbol['va']
        calls.append(dict(caller=caller, target=callee, target_owner=callee_symbol['owner'], va=address))
    cursor = json.loads((A / 'shard-1/wow-cursor-evidence.json').read_text(encoding='utf-8'))
    recorded_release = next(x for x in cursor['inputs'] if x['path'].replace('\\', '/').lower() == 'assets/release/wow32.dll')
    assert recorded_release['sha256'] == sha(release)
    assert any(r['ordinal'] == 533 and r['target'] == '_WU32NotifyWow@4' for r in cursor['initial_dispatch'])
    assert any(s['name'] == '_WU32NotifyWow@4' and s['owner'] == 'wuman.obj' for s in symbols)
    return dict(result='pass', relative_path=REL, sha256=sha(source), wow_source_ids=f['wow_source_ids'],
        edge=edge, dependency=dependency, functions=functions, calls=calls,
        inputs=[dict(path=str(p), sha256=sha(p)) for p in
            (source, original, original45, release, B / 'wow32.dll', B / 'wow32.dll.map', B / 'build.ninja',
             B / target, B / 'obj/wuman.obj', B / 'obj/wkman.obj', A / 'shard-1/wow-cursor-evidence.json')],
        scope='Selected original code and initial call edges; no guest execution, invalid-input or alias lifetime proof')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
