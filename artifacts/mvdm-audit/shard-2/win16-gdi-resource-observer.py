"""Read-only Win16 GDI guest resource and delivered image evidence."""
import csv, hashlib, json, zipfile
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
REL = 'wow16/gdi/gdi.rcv'

def digest(data):
    return hashlib.sha256(data).hexdigest().upper()

def sha(p):
    return digest(p.read_bytes()) if p.is_file() else ''

def inspect():
    frozen = {r['relative_path']: r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    f = frozen[REL]; hashes = [sha(root / REL) for root in (M, *O)]
    assert hashes == [f['current_sha256'], f['opennt_sha256'], f['opennt45_sha256']]
    assert hashes[0] and hashes[0] == hashes[1] == hashes[2]
    names = ('wow16/gdi/makefile', 'wow16/gdi/gdi.rc', 'wow16/gdi/gdi.asm')
    ctx = [M / n for n in names]
    assert all(sha(p) == sha(O[0] / p.relative_to(M)) for p in ctx)
    make, rc, asm = [p.read_text(encoding='latin1').lower() for p in ctx]
    assert '#include "gdi.rcv"' in rc and 'gdi.res: $*.rc $*.rcv' in make
    assert 'rc16 -t gdi.res gdi.exe' in make and 'gdi.obj' in make and 'wowgdi.inc' in asm
    archive = Path('assets/winnt.zip')
    with zipfile.ZipFile(archive) as z:
        members = [n for n in z.namelist() if n.lower() == 'winnt/system32/gdi.exe']
        assert len(members) == 1
        image = digest(z.read(members[0]))
    source = M / 'wow16/gdi/GDI.EXE'; installed = Path('O:/winnt/system32/GDI.EXE')
    return dict(result='pass', relative_path=REL, hashes=hashes, dos_catalog_rows=f['dos_catalog_rows'],
                wow_catalog_rows=f['wow_catalog_rows'], context=[dict(path=str(p), sha256=sha(p)) for p in ctx],
                archive_sha256=sha(archive), archive_member=members[0], image_sha256=image,
                source_image_sha256=sha(source), installed_path=str(installed), installed_sha256=sha(installed),
                scope='Historical GDI resource input and image identities, not source-to-image or drawing behavior')

if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
