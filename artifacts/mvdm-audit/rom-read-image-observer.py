"""Bind original resource reader's selected body to the released worker."""
import hashlib
import json
import runpy
from pathlib import Path


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    h = runpy.run_path('artifacts/mvdm-audit/host-tail-image-observer.py')
    build = Path('build/.ninja/msvc-x86-release/worker')
    release = Path('assets/release/ntvdm.exe')
    assert sha(release) == sha('O:/winnt/system32/ntvdm.exe')
    symbols = h['symbols']((build / 'ntvdm.exe.map').read_text())
    symbol = next(x for x in symbols if x['name'] == '_host_read_resource' and x['function'] and
                  x['owner'] == 'original-softpc-host-roots:nt_rez.obj')
    obj = build / 'obj/host/nt_rez.obj'
    binding = h['bind'](h['Image'](release), h['Coff'](obj), symbol)
    return dict(result='pass', source_sha256=sha('src/mvdm/softpc.new/host/src/nt_rez.c'),
                object_sha256=sha(obj), map_sha256=sha(build / 'ntvdm.exe.map'), release_sha256=sha(release),
                binding=binding, scope='Relocation-masked selected function body; no actual file or guest execution')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
