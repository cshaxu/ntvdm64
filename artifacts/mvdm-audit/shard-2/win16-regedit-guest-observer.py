"""Read-only REGEDIT guest source/build role and executable package observations."""
import csv, hashlib, json, zipfile
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
ROOT = 'wow16/regedit/'
NAMES = ('debug/REGEDT16.EXE', 'reg1632.h', 'sdkreged.def')

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
        rows.append(dict(path=rel, hashes=hashes, dos_catalog_rows=f['dos_catalog_rows'], wow_catalog_rows=f['wow_catalog_rows']))
    ctx = [M / (ROOT + n) for n in ('makefile', 'regporte.c', 'regedit.def')]
    assert all(sha(p) == sha(O[0] / p.relative_to(M)) for p in ctx)
    make, implementation, definition = [p.read_text(encoding='latin1').lower() for p in ctx]
    assert 'regporte.obj: regporte.c reg1632.h' in make and '#include "reg1632.h"' in implementation
    assert '$(dstdir)\\regporte.obj' in make and '$(common2)' in make
    assert 'regopenkey(' in implementation and 'regsetvalue' in implementation
    assert 'regedit.def' in make and 'sdkreged.def' not in make
    archive = Path('assets/winnt.zip')
    with zipfile.ZipFile(archive) as z:
        matches = [dict(member=n, sha256=digest(z.read(n))) for n in z.namelist() if Path(n).name.lower() == 'regedt16.exe']
    installed = Path('O:/winnt/system32/REGEDT16.EXE')
    return dict(result='pass', sources=rows, context=[dict(path=str(p), sha256=sha(p)) for p in ctx],
                archive_sha256=sha(archive), archive_basename_matches=matches,
                installed_path=str(installed), installed_sha256=sha(installed),
                scope='Original guest build references and package identities; no runtime registry behavior or source-to-image claim')

if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
