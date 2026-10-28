"""Read-only WRITE guest source, variant build and image observations."""
import csv, hashlib, json, zipfile
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
ROOT = 'wow16/write/'
NAMES = ('dtabbtn.bms','dtabmark.bms','editsect.c','form1.c','help.c','just.bms',
         'kmake.bat','looks.h','sp15.bms','state.rst','WRITE.EXE','WRITE.RES','write16.exe')

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
    ctx = [M / (ROOT + n) for n in ('makefile', 'write.lnk')]
    assert all(sha(p) == sha(O[0] / p.relative_to(M)) for p in ctx)
    make, link = [p.read_text(encoding='latin1').lower() for p in ctx]
    assert 'editsect.obj:  editsect.c' in make and 'form1.obj:     form1.c' in make and 'help.obj:      help.c' in make
    assert '!ifdef  dbcs' in make and 'obj_dbcs=help.obj' in make and 'format.obj:    d_form1.c' in make
    assert 'link16 @$(app).lnk' in make and 'rc16 $(rcflags) $(app).res $(app).exe' in make
    assert 'copy write.exe write16.exe' in make and 'format.obj' in link
    state = (M / (ROOT + 'state.rst')).read_text(encoding='latin1').lower()
    assert '[edit-]' in state and 'p:\\src\\apps\\write\\obj.c' in state
    archive = Path('assets/winnt.zip')
    with zipfile.ZipFile(archive) as z:
        matches = [dict(member=n, sha256=digest(z.read(n))) for n in z.namelist() if Path(n).name.lower() in ('write.exe','write16.exe')]
    installed = Path('O:/winnt/WRITE.EXE')
    return dict(result='pass', sources=rows, context=[dict(path=str(p), sha256=sha(p)) for p in ctx],
                archive_sha256=sha(archive), archive_images=matches,
                installed_path=str(installed), installed_sha256=sha(installed),
                scope='Original WRITE build-variant references and image identities; no runtime behavior or image lineage claim')

if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
