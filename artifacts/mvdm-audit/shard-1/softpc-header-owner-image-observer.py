"""Bind selected SoftPC header dependents to released worker function bodies."""
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
    family_path = A / 'shard-1/softpc-header-family-evidence.json'
    family = json.loads(family_path.read_text(encoding='utf-8-sig'))
    rows = {x['name']: x for x in family['files']}
    names = ('dsktrace.h', 'gendrvr.h', 'host_gwi.h', 'host_sas.h', 'hunter.h')
    requirements = {'dsktrace.h': 'obj/disks/diskbios.obj', 'gendrvr.h': 'obj/bios/reset.obj',
                    'host_gwi.h': 'obj/bios/reset.obj', 'host_sas.h': 'obj/bios/reset.obj',
                    'hunter.h': 'obj/bios/reset.obj'}
    for name in names:
        row = rows[name]
        assert row['reference_hashes'] == [row['sha256'], row['sha256']]
        assert requirements[name] in row['valid_worker_objects']
    image_path = Path('assets/release/ntvdm.exe')
    assert sha(image_path) == sha('O:/winnt/system32/ntvdm.exe')
    image = h['Image'](image_path)
    symbols = h['symbols']((B / 'ntvdm.exe.map').read_text())
    bindings = []
    for name, objname, owner in (('_reset', 'obj/bios/reset.obj', 'original-softpc-bios:reset.obj'),
                                  ('_disk_io', 'obj/disks/diskbios.obj', 'original-softpc-disks:diskbios.obj')):
        symbol = next(s for s in symbols if s['name'] == name and s['owner'] == owner and s['function'])
        bindings.append(h['bind'](image, h['Coff'](B / objname), symbol))
    return dict(result='pass', family_sha256=sha(family_path), names=list(names), dependencies=requirements,
                bindings=bindings, release_sha256=sha(image_path), map_sha256=sha(B / 'ntvdm.exe.map'),
                scope='Selected dependent function bodies only; no proof every header declaration is active or runtime behavior')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
