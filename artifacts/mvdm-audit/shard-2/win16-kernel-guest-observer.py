"""Read-only original Win16 kernel file roles and delivered binary identity."""
import csv, hashlib, json, zipfile
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
ROOT = 'wow16/kernel31/'
NAMES = ('KRNL386.RES', 'ksource.txt', 'newexe.doc', 'retail3/krnl386.exe')

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
    ctx = [M / (ROOT + n) for n in ('makefile', 'krnl386.rc', 'krnl386.lnk')]
    assert all(sha(p) == sha(O[0] / p.relative_to(M)) for p in ctx)
    make, rc, link = [p.read_text(encoding='latin1').lower() for p in ctx]
    assert 'dest=retail3' in make and 'rc16 -t ..\\krnl386.res kernel.exe' in make
    assert 'krnl386.res: krnl386.rc' in make and 'krnl386.exe' in rc and 'kernel.exe/align' in link
    assert '3.1 kernel directory' in (M / (ROOT + 'ksource.txt')).read_text(encoding='latin1').lower()
    assert 'new executable format' in (M / (ROOT + 'newexe.doc')).read_text(encoding='latin1').lower()
    archive = Path('assets/winnt.zip')
    with zipfile.ZipFile(archive) as z:
        members = [n for n in z.namelist() if n.lower() == 'winnt/system32/krnl386.exe']
        assert len(members) == 1
        image = digest(z.read(members[0]))
    installed = Path('O:/winnt/system32/KRNL386.EXE')
    return dict(result='pass', sources=rows, context=[dict(path=str(p), sha256=sha(p)) for p in ctx],
                archive_sha256=sha(archive), archive_member=members[0], archive_image_sha256=image,
                installed_path=str(installed), installed_sha256=sha(installed),
                scope='Historical guest inputs and image identities; no deployed image provenance or runtime assertion')

if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
