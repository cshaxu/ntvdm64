"""Read-only CTL3D original debugging-source and shipped-image observations."""
import csv, hashlib, json, zipfile
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
ROOT = 'wow16/ctl3dv2/'
NAMES = ('common.ver', 'ctl3d.c', 'ctl3d.doc', 'ctl3d.h', 'msdn.ico', 'readme.txt')

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
    ctx = [M / (ROOT + 'ctl3d.rc'), M / 'bin86/makefile', M / (ROOT + 'new3d.doc')]
    assert all(sha(p) == sha(O[0] / p.relative_to(M)) for p in ctx)
    rc, binmake = [p.read_text(encoding='latin1').lower() for p in ctx[:2]]
    readme = (M / (ROOT + 'readme.txt')).read_text(encoding='latin1').lower()
    source = (M / (ROOT + 'ctl3d.c')).read_text(encoding='latin1').lower()
    assert 'for debugging purposes only' in readme and 'bin86' in readme and 'special_wow_version' in readme
    assert '#include "common.ver"' in rc and '#include "ctl3d.h"' in rc and '#include "ctl3d.h"' in source
    assert 'binplace ctl3dv2.dll' in binmake and sha(M / (ROOT + 'ctl3d.doc')) == sha(ctx[2])
    archive = Path('assets/winnt.zip')
    with zipfile.ZipFile(archive) as z:
        names = [n for n in z.namelist() if n.lower() == 'winnt/system32/ctl3dv2.dll']
        assert len(names) == 1
        image = digest(z.read(names[0]))
    source_image = M / 'bin86/ctl3dv2.dll'; installed = Path('O:/winnt/system32/CTL3DV2.DLL')
    assert image == sha(source_image)
    return dict(result='pass', sources=rows, context=[dict(path=str(p), sha256=sha(p)) for p in ctx],
                archive_sha256=sha(archive), archive_member=names[0], image_sha256=image,
                installed_path=str(installed), installed_sha256=sha(installed),
                scope='Original debugging-material description and package identity, not source-to-binary or control behavior')

if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
