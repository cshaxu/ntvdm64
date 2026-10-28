"""Read-only display/keyboard guest resource roles and image observations."""
import csv, hashlib, json, zipfile
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
NAMES = ('wow16/drivers/display/config.LST', 'wow16/drivers/display/display.rc',
         'wow16/drivers/display/DISPLAY.RES', 'wow16/drivers/keyboard/keyboard.rcv')

def digest(data):
    return hashlib.sha256(data).hexdigest().upper()

def sha(p):
    return digest(p.read_bytes()) if p.is_file() else ''

def inspect():
    frozen = {r['relative_path']: r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    rows = []
    for rel in NAMES:
        f = frozen[rel]; hashes = [sha(root / rel) for root in (M, *O)]
        assert hashes == [f['current_sha256'], f['opennt_sha256'], f['opennt45_sha256']]
        rows.append(dict(path=rel, hashes=hashes, dos_catalog_rows=f['dos_catalog_rows'], wow_catalog_rows=f['wow_catalog_rows']))
    ctx_names = ('wow16/drivers/display/makefile', 'wow16/drivers/keyboard/makefile', 'wow16/drivers/keyboard/keyboard.rc')
    ctx = [M / n for n in ctx_names]
    assert all(sha(p) == sha(O[0] / p.relative_to(M)) for p in ctx)
    display_make, key_make, key_rc = [p.read_text(encoding='latin1').lower() for p in ctx]
    display_rc = (M / NAMES[1]).read_text(encoding='latin1').lower()
    assert 'display.res: $*.rc config.bin $*.rcv' in display_make and 'rc16 -t display.res display.exe' in display_make
    assert 'copy display.exe vga.drv' in display_make and 'config.bin' in display_rc
    assert 'keyboard.res: $*.rc $*.rcv' in key_make and '#include "keyboard.rcv"' in key_rc
    archive = Path('assets/winnt.zip'); images = []
    with zipfile.ZipFile(archive) as z:
        for name, source in (('VGA.DRV', 'wow16/drivers/display/vga.drv'), ('KEYBOARD.DRV', 'wow16/drivers/keyboard/keyboard.drv')):
            matches = [n for n in z.namelist() if n.lower() == ('winnt/system32/' + name).lower()]
            assert len(matches) <= 1
            installed = Path('O:/winnt/system32') / name
            images.append(dict(name=name, archive_member=matches[0] if matches else '',
                               archive_sha256=digest(z.read(matches[0])) if matches else '',
                               source_sha256=sha(M / source), installed_sha256=sha(installed)))
    return dict(result='pass', sources=rows, context=[dict(path=str(p), sha256=sha(p)) for p in ctx],
                archive_sha256=sha(archive), images=images,
                scope='Historical guest driver resource role and image identities, not runtime or image provenance')

if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
