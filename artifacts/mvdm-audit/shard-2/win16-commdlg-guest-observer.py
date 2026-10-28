"""Inspect original COMMDLG guest build roles and delivered image, without running it."""
import csv, hashlib, json, re, zipfile
from pathlib import Path

A = Path('artifacts/mvdm-audit')
M = Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
ROOT = 'wow16/commdlg/'
NAMES = ('colordlg.h', 'COMMDLG.DLL', 'parse.h', 'privcomd.h', 'smfldr2.bmp', 'smfldr3.bmp', 'STRIPPED.DEF')

def sha_bytes(data):
    return hashlib.sha256(data).hexdigest().upper()

def sha(p):
    return sha_bytes(p.read_bytes()) if p.is_file() else ''

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
    paths = [M / (ROOT + n) for n in ('makefile', 'color.dlg', 'parse.c', 'font.c', 'commdlg.rc')]
    assert all(sha(p) == sha(O[0] / p.relative_to(M)) for p in paths)
    make, color, parse, font, resource = [p.read_text(encoding='latin1').lower() for p in paths]
    assert '#include "colordlg.h"' in color and '#include "parse.h"' in parse and '#include "privcomd.h"' in font
    assert 'mkpublic commdlg.def stripped.def' in make and 'implib commdlg.lib stripped.def' in make
    assert 'parse.obj' in make and 'font.obj' in make and 'commdlg.dll' in make
    assert 'filebmps.bmp' in resource and 'smfldr2.bmp' not in resource and 'smfldr3.bmp' not in resource
    archive = Path('assets/winnt.zip')
    with zipfile.ZipFile(archive) as z:
        members = [n for n in z.namelist() if n.lower() == 'winnt/system32/commdlg.dll']
        assert len(members) == 1
        image = sha_bytes(z.read(members[0]))
    assert image == sha(M / (ROOT + 'COMMDLG.DLL'))
    installed = Path('O:/winnt/system32/COMMDLG.DLL')
    return dict(result='pass', sources=rows, context=[dict(path=str(p), sha256=sha(p)) for p in paths],
                archive_sha256=sha(archive), archive_member=members[0], image_sha256=image,
                installed_path=str(installed), installed_sha256=sha(installed),
                scope='Historical guest input and shipped image identity only; no source-to-image or runtime conclusion')

if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
