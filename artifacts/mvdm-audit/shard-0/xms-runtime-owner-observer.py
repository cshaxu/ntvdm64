"""Bind selected original XMS owner functions to released worker code."""
import csv
import hashlib
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
B = Path('build/.ninja/msvc-x86-release/worker')
M = Path('src/mvdm')
ORIGINALS = (Path('O:/repos.external/OpenNT/base/mvdm'),
             Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
OWNERS = (
    ('xms.486/xms.c', 'xms.obj', ('_XMSInit',)),
    ('xms.486/xmsa20.c', 'xmsa20.obj', ('_xmsA20', '_xmsEnableA20Wrapping', '_xmsDisableA20Wrapping')),
    ('xms.486/xmsmisc.c', 'xmsmisc.obj', ('_xmsNotifyHookI15', '_xmsQueryExtMem', '_xmsSysPageSize')),
    ('xms.486/xmsumb.c', 'xmsumb.obj', ('_xmsInitUMB', '_xmsRequestUMB', '_xmsReleaseUMB')),
)


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()


def inspect():
    with (A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig', newline='') as f:
        frozen = {r['relative_path']: r for r in csv.DictReader(f)}
    h = runpy.run_path(str(A / 'host-tail-image-observer.py'))
    image = h['Image'](Path('assets/release/ntvdm.exe'))
    cached = h['Image'](B / 'ntvdm.exe')
    assert image.code == cached.code and all(image.data[a:b] == cached.data[a:b] for a, b, _ in image.code)
    symbols = h['symbols']((B / 'ntvdm.exe.map').read_text())
    result = []
    for source, name, names in OWNERS:
        hashes = [sha(root / source) for root in (M, *ORIGINALS)]
        assert hashes == [frozen[source]['current_sha256'], frozen[source]['opennt_sha256'], frozen[source]['opennt45_sha256']]
        objpath = B / 'obj/xms' / name
        obj = h['Coff'](objpath)
        bindings = []
        for name2 in names:
            symbol = next(s for s in symbols if s['name'] == name2 and s['owner'] == 'original-mvdm-xms:' + name)
            bindings.append(h['bind'](image, obj, symbol))
        result.append(dict(path=source, hashes=hashes, object_sha256=sha(objpath), bindings=bindings))
    return dict(result='pass', sources=result, image_sha256=sha('assets/release/ntvdm.exe'),
        cached_sha256=sha(B / 'ntvdm.exe'), map_sha256=sha(B / 'ntvdm.exe.map'),
        limits='Representative function binding only; no XMS guest behavior, errors or lifecycle proof.')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
