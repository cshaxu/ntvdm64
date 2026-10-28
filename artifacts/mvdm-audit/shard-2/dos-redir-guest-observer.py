"""Read-only redirector historical recipe and delivered guest identity."""
import csv, hashlib, json, zipfile
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
ROOT = 'dos/v86/redir/'
NAMES = ('fudgedef.inc', 'obj/redir.exe')

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
    names = (ROOT + 'makefile', ROOT + 'redir.asm', 'inc/rdrsvc.h', 'inc/bop.h')
    ctx = [M / n for n in names]
    assert all(sha(p) == sha(O[0] / p.relative_to(M)) for p in ctx)
    make, guest, service, bop = [p.read_text(encoding='latin1').lower() for p in ctx]
    assert 'target      = $(objpath)\\redir.exe' in make and '$(target):  $(objs)' in make
    assert 'include rdrsvc.inc' in guest and 'include bop.inc' in service and 'bop_redir' in service
    assert '#define bop_redir            0x57' in bop
    archive = Path('assets/winnt.zip')
    with zipfile.ZipFile(archive) as z:
        matches = [dict(member=n, sha256=digest(z.read(n))) for n in z.namelist() if Path(n).name.lower() == 'redir.exe']
    installed = Path('O:/winnt/system32/REDIR.EXE')
    return dict(result='pass', sources=rows, context=[dict(path=str(p), sha256=sha(p)) for p in ctx],
                archive_sha256=sha(archive), archive_basename_matches=matches,
                installed_path=str(installed), installed_sha256=sha(installed),
                scope='Historical recipe and BOP declarations, package observations, not live redirector behavior')

if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
