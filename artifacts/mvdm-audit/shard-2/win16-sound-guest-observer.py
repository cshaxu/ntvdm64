"""Read-only SOUND.DRV identity and original guest-to-host dispatch edges."""
import csv, hashlib, json, re, zipfile
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
REL = 'wow16/drivers/sound/sound.drv'

def digest(data):
    return hashlib.sha256(data).hexdigest().upper()

def sha(p):
    return digest(p.read_bytes()) if p.is_file() else ''

def inspect():
    rows = {r['relative_path']: r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    f = rows[REL]; hashes = [sha(root / REL) for root in (M, *O)]
    assert hashes == [f['current_sha256'], f['opennt_sha256'], f['opennt45_sha256']]
    assert hashes[0] == hashes[1] == hashes[2] and hashes[0]
    names = ('wow16/drivers/sound/makefile', 'wow16/drivers/sound/sound.def',
             'wow16/drivers/sound/sound.asm', 'wow32/wstbl2.h', 'wow32/wsman.c')
    ctx = [M / n for n in names]
    assert all(sha(p) == sha(O[0] / p.relative_to(M)) for p in ctx)
    make, definition, guest, table, impl = [p.read_text(encoding='latin1').lower() for p in ctx]
    assert 'sound.drv: $*.obj $*.lrf $*.def $*.res' in make and 'link16 @$*.lrf' in make
    assert re.search(r'dobeep\s+@17\b', definition) and re.search(r'soundthunk\s+dobeep\b', guest)
    assert 'ws32dobeep' in table and 'unimplementedapi' in table and 'messagebeep(0)' in impl
    archive = Path('assets/winnt.zip')
    with zipfile.ZipFile(archive) as z:
        members = [n for n in z.namelist() if n.lower() == 'winnt/system32/sound.drv']
        assert len(members) == 1
        image = digest(z.read(members[0]))
    installed = Path('O:/winnt/system32/SOUND.DRV')
    assert image == hashes[0] == sha(installed)
    return dict(result='pass', relative_path=REL, hashes=hashes,
                dos_catalog_rows=f['dos_catalog_rows'], wow_catalog_rows=f['wow_catalog_rows'],
                context=[dict(path=str(p), sha256=sha(p)) for p in ctx],
                archive_sha256=sha(archive), archive_member=members[0], image_sha256=image,
                installed_path=str(installed), installed_sha256=sha(installed),
                scope='Guest image identity and source-level dispatch; no live module selection or audio witness')

if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
