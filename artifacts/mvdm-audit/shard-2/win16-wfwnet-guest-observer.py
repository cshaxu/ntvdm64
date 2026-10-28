"""Read-only WFWNET original guest build inputs and shipped-image identity."""
import csv, hashlib, json, zipfile
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
ROOT = 'wow16/wfwnet/'
NAMES = ('bseerr.h', 'libentry.asm', 'wfwnet.c', 'wfwnet.rcv', 'WFWNET.RES')

def digest(data):
    return hashlib.sha256(data).hexdigest().upper()

def sha(p):
    return digest(p.read_bytes()) if p.is_file() else ''

def inspect():
    frozen = {r['relative_path']: r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    rows = []
    for name in NAMES:
        rel = ROOT + name; f = frozen[rel]
        hashes = [sha(root / rel) for root in (M, *O)]
        assert hashes == [f['current_sha256'], f['opennt_sha256'], f['opennt45_sha256']]
        assert hashes[0] and hashes[0] in hashes[1:]
        rows.append(dict(path=rel, hashes=hashes, dos_catalog_rows=f['dos_catalog_rows'], wow_catalog_rows=f['wow_catalog_rows']))
    ctx = [M / (ROOT + n) for n in ('makefile', 'locals.h', 'wfwnet.rc')]
    assert all(sha(p) == sha(O[0] / p.relative_to(M)) for p in ctx)
    make, local, rc = [p.read_text(encoding='latin1').lower() for p in ctx]
    impl = (M / (ROOT + 'wfwnet.c')).read_text(encoding='latin1').lower()
    entry = (M / (ROOT + 'libentry.asm')).read_text(encoding='latin1').lower()
    assert '#include <locals.h>' in impl and '#include <bseerr.h>' in local
    assert 'wfwnet.obj: wfwnet.c' in make and 'libentry.obj' in make and 'wfwnet.res' in make
    assert 'call    libmain' in entry and '#include "wfwnet.rcv"' in rc and 'rc16 -t wfwnet.res wfwnet.drv' in make
    archive = Path('assets/winnt.zip')
    with zipfile.ZipFile(archive) as z:
        matches = [n for n in z.namelist() if n.lower() == 'winnt/system32/wfwnet.drv']
        assert len(matches) == 1
        image = digest(z.read(matches[0]))
    source = M / (ROOT + 'wfwnet.drv'); installed = Path('O:/winnt/system32/WFWNET.DRV')
    return dict(result='pass', sources=rows, context=[dict(path=str(p), sha256=sha(p)) for p in ctx],
                archive_sha256=sha(archive), archive_member=matches[0], image_sha256=image,
                source_image_sha256=sha(source), installed_path=str(installed), installed_sha256=sha(installed),
                scope='Original guest build roles and image identity, not network or spool runtime behavior')

if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
