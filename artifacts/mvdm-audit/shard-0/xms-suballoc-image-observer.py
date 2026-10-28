"""Bind selected XMS dispatcher and suballocator bodies to release NTVDM."""
import hashlib
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit'); B = Path('build/.ninja/msvc-x86-release/worker')
def sha(p): return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()
def inspect():
    h = runpy.run_path(str(A / 'host-tail-image-observer.py'))
    image = h['Image'](Path('assets/release/ntvdm.exe')); cached = h['Image'](B / 'ntvdm.exe')
    assert image.code == cached.code and all(image.data[a:b] == cached.data[a:b] for a,b,_ in image.code)
    symbols = h['symbols']((B / 'ntvdm.exe.map').read_text()); result = []
    for objname, owner, names in (('obj/xms/xmsdisp.obj', 'original-mvdm-xms:xmsdisp.obj', ('_XMSDispatch',)),
                                  ('obj/suballoc/suballoc.obj', 'original-mvdm-host-suballoc:suballoc.obj', ('_SAAllocate', '_SAInitialize'))):
        obj = h['Coff'](B / objname)
        for name in names:
            symbol = next(x for x in symbols if x['name'] == name and x['owner'] == owner)
            result.append(h['bind'](image, obj, symbol))
    return dict(result='pass', bindings=result, image_sha256=sha('assets/release/ntvdm.exe'),
                map_sha256=sha(B / 'ntvdm.exe.map'),
                object_sha256={n: sha(B / n) for n in ('obj/xms/xmsdisp.obj', 'obj/suballoc/suballoc.obj')},
                limits='Object/image binding only; no guest index bounds, allocation failure, rollback or lifecycle execution.')
if __name__ == '__main__': print(json.dumps(inspect(), indent=2))
