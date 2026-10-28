"""Read-only original MCIOLE guest ownership, linkage, and package identity."""
import csv, hashlib, json, re, zipfile
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
ROOT = 'wow16/mciole/'
NAMES = ('libinit.asm', 'MCIOLE16.DLL', 'mciole16.rcv')

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
    ctx = [M / (ROOT + n) for n in ('makefile', 'mciole16.rc', 'mciole.def', 'mciole.c')]
    assert all(sha(p) == sha(O[0] / p.relative_to(M)) for p in ctx)
    make, resource, definition, impl = [p.read_text(encoding='latin1').lower() for p in ctx]
    assert re.search(r'obj1\s*=\s*libinit\.obj\s+mciole\.obj', make)
    assert 'mciole16.dll: $(obj1) mciole16.res' in make and 'mciole.def' in make
    assert '#include "mciole16.rcv"' in resource
    assert 'dllloadfromstream' in definition and 'olecli.defloadfromstream' in definition
    assert '#include "mciole.h"' in impl
    archive = Path('assets/winnt.zip')
    with zipfile.ZipFile(archive) as z:
        members = [n for n in z.namelist() if n.lower() == 'winnt/system32/mciole16.dll']
        assert len(members) == 1
        image = digest(z.read(members[0]))
    assert image == sha(M / (ROOT + 'MCIOLE16.DLL'))
    installed = Path('O:/winnt/system32/MCIOLE16.DLL')
    return dict(result='pass', sources=rows, context=[dict(path=str(p), sha256=sha(p)) for p in ctx],
                archive_sha256=sha(archive), archive_member=members[0], image_sha256=image,
                installed_path=str(installed), installed_sha256=sha(installed),
                scope='Historical guest build/exports and image identity; no runtime functionality or build provenance claim')

if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
