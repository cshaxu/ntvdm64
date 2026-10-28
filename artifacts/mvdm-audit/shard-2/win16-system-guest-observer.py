"""Inspect historical SYSTEM guest build variants and packaged image identity."""
import csv, hashlib, json, re, zipfile
from pathlib import Path

A = Path('artifacts/mvdm-audit')
M = Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
ROOT = 'wow16/system/'
NAMES = ('romsys.def', 'system', 'system.lrf', 'system.rcv', 'vecsys.inc', 'windefs.inc')

def digest(data):
    return hashlib.sha256(data).hexdigest().upper()

def sha(p):
    return digest(p.read_bytes()) if p.is_file() else ''

def inspect():
    frozen = {r['relative_path']: r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    rows = []
    for name in NAMES:
        rel = ROOT + name
        f = frozen[rel]
        hashes = [sha(root / rel) for root in (M, *O)]
        assert hashes == [f['current_sha256'], f['opennt_sha256'], f['opennt45_sha256']]
        assert hashes[0]
        if name != 'system.lrf':
            assert hashes[0] in hashes[1:]
        rows.append(dict(path=rel, hashes=hashes, dos_catalog_rows=f['dos_catalog_rows'], wow_catalog_rows=f['wow_catalog_rows']))
    ctx = [M / (ROOT + n) for n in ('makefile', 'romsys', 'hpsystem', 'envect.asm', 'system.inc', 'system.rc')]
    assert all(sha(p) == sha(O[0] / p.relative_to(M)) for p in ctx)
    make, rom, hp, envect, includes, resource = [p.read_text(encoding='latin1').lower() for p in ctx]
    assert 'system.lrf: makefile' in make and 'link16 @system.lrf' in make
    assert 'romsys.def' in rom and 'vecsys.inc' in hp and re.search(r'(?m)^\s*include\s+vecsys\.inc\b', envect)
    assert re.search(r'(?m)^\s*;\s*include\s+windefs\.inc', includes)
    assert '#include "system.rcv"' in resource
    alt = (M / (ROOT + 'system')).read_text(encoding='latin1').lower()
    assert 'system.def system.lnk system.obj timer.obj' in alt
    archive = Path('assets/winnt.zip')
    with zipfile.ZipFile(archive) as z:
        names = [n for n in z.namelist() if n.lower() == 'winnt/system32/system.drv']
        assert len(names) == 1
        image = digest(z.read(names[0]))
    image_path = M / (ROOT + 'system.drv')
    installed = Path('O:/winnt/system32/SYSTEM.DRV')
    return dict(result='pass', sources=rows, context=[dict(path=str(p), sha256=sha(p)) for p in ctx],
                archive_sha256=sha(archive), archive_member=names[0], archive_image_sha256=image,
                source_image_sha256=sha(image_path), installed_path=str(installed), installed_sha256=sha(installed),
                scope='Historical guest build variants and binary identity; no guest runtime or image provenance claim')

if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
