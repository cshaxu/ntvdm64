"""Read-only identity and historical build edges for the Win16 timer driver."""
import csv, hashlib, json, re, zipfile
from pathlib import Path

A = Path('artifacts/mvdm-audit')
M = Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
ROOT = 'wow16/timer/'
NAMES = ('libinit.asm', 'local.asm', 'math.asm', 'sysinfo.inc', 'TIMER.DRV', 'timer.inc')

def sha_bytes(data):
    return hashlib.sha256(data).hexdigest().upper()

def sha(path):
    return sha_bytes(path.read_bytes()) if path.is_file() else ''

def inspect():
    frozen = {r['relative_path']: r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    rows = []
    for name in NAMES:
        rel = ROOT + name
        f = frozen[rel]
        hashes = [sha(root / rel) for root in (M, *O)]
        assert hashes == [f['current_sha256'], f['opennt_sha256'], f['opennt45_sha256']]
        assert hashes[0] and hashes[0] in hashes[1:]
        rows.append(dict(path=rel, hashes=hashes, dos_catalog_rows=f['dos_catalog_rows'], wow_catalog_rows=f['wow_catalog_rows']))
    context = [M / (ROOT + n) for n in ('makefile', 'timer.def')]
    assert all(sha(p) == sha(O[0] / p.relative_to(M)) == sha(O[1] / p.relative_to(M)) for p in context)
    make = context[0].read_text(encoding='latin1').lower()
    assert all(n in make for n in ('$zlibinit.obj', '$zlocal.obj', '$zmath.obj', 'link16', 'timer.drv'))
    for n in ('libinit.asm', 'local.asm', 'math.asm'):
        assert re.search(r'(?mi)^\s*include\s+timer\.inc\b', (M / (ROOT + n)).read_text(encoding='latin1'))
    assert re.search(r'(?mi)^\s*include\s+sysinfo\.inc\b', (M / (ROOT + 'libinit.asm')).read_text(encoding='latin1'))
    definition = (M / (ROOT + 'timer.def')).read_text(encoding='latin1').lower()
    assert re.search(r'wep\s+@1', definition) and re.search(r'driverproc\s+@2', definition)
    archive = Path('assets/winnt.zip')
    with zipfile.ZipFile(archive) as z:
        found = [n for n in z.namelist() if n.lower() == 'winnt/system32/timer.drv']
        assert len(found) == 1
        image_hash = sha_bytes(z.read(found[0]))
    assert image_hash == sha(M / (ROOT + 'TIMER.DRV'))
    installed = Path('O:/winnt/system32/TIMER.DRV')
    return dict(result='pass', sources=rows, context=[dict(path=str(p), sha256=sha(p)) for p in context], archive_sha256=sha(archive), archive_member=found[0],
                image_sha256=image_hash, installed_path=str(installed), installed_sha256=sha(installed),
                scope='Historical guest source and package identity; no runtime or build lineage claim')

if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
