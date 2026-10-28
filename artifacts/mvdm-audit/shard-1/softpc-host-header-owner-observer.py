"""Bind dependent original host timer and COM functions to released worker."""
import hashlib
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
B = Path('build/.ninja/msvc-x86-release/worker')


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    h = runpy.run_path(str(A / 'host-tail-image-observer.py'))
    family = A / 'shard-1/softpc-host-header-family-evidence.json'
    rows = {x['name']: x for x in json.loads(family.read_text(encoding='utf-8-sig'))['files']}
    dependencies = {'host_rrr.h': 'obj/host/nt_timer.obj', 'nt_eoi.h': 'obj/host/nt_timer.obj',
                    'nt_pif.h': 'obj/host/nt_timer.obj', 'host_trc.h': 'obj/host/nt_com.obj'}
    for name, obj in dependencies.items():
        assert rows[name]['reference_hashes'] == [rows[name]['sha256'], rows[name]['sha256']]
        assert obj in rows[name]['valid_worker_objects']
    image_path = Path('assets/release/ntvdm.exe')
    assert sha(image_path) == sha('O:/winnt/system32/ntvdm.exe')
    image = h['Image'](image_path)
    symbols = h['symbols']((B / 'ntvdm.exe.map').read_text())
    bindings = []
    for name, objname, owner in (('_TimerInit', 'obj/host/nt_timer.obj', 'original-softpc-host-roots:nt_timer.obj'),
                                  ('_FlushTXBuffer', 'obj/host/nt_com.obj', 'original-softpc-host-roots:nt_com.obj')):
        symbol = next(x for x in symbols if x['name'] == name and x['owner'] == owner and x['function'])
        bindings.append(h['bind'](image, h['Coff'](B / objname), symbol))
    return dict(result='pass', family_sha256=sha(family), dependencies=dependencies, bindings=bindings,
                release_sha256=sha(image_path), map_sha256=sha(B / 'ntvdm.exe.map'),
                scope='Dependent function bodies only; no proof individual header declarations or runtime device behavior')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
