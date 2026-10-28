"""Read-only Win16 OLE client build and deployment evidence."""
import csv, hashlib, json, zipfile
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
ROOT = 'wow16/ole/client/'
NAMES = ('funchead.c', 'mk.cmd', 'utils.c')

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
    ctx = [M / (ROOT + n) for n in ('makefile', 'bm.c', 'dll.h')]
    assert all(sha(p) == sha(O[0] / p.relative_to(M)) for p in ctx)
    make, bm, decl = [p.read_text(encoding='latin1').lower() for p in ctx]
    utils = (M / (ROOT + 'utils.c')).read_text(encoding='latin1').lower()
    script = (M / (ROOT + 'mk.cmd')).read_text(encoding='latin1').lower()
    assert '$(bld)\\utils.obj' in make and '$(bld)\\olecli.dll: $(le_obj)' in make
    assert 'putstrwithlen' in bm and 'putstrwithlen' in decl and 'putstrwithlen' in utils
    assert 'nmake  >z.msg' in script and 'set debug=' in script
    archive = Path('assets/winnt.zip')
    with zipfile.ZipFile(archive) as z:
        names = [n for n in z.namelist() if n.lower() == 'winnt/system32/olecli.dll']
        assert len(names) == 1
        image = digest(z.read(names[0]))
    source_image = M / 'bin86/olecli.dll'; installed = Path('O:/winnt/system32/OLECLI.DLL')
    return dict(result='pass', sources=rows, context=[dict(path=str(p), sha256=sha(p)) for p in ctx],
                archive_sha256=sha(archive), archive_member=names[0], image_sha256=image,
                source_image_sha256=sha(source_image), installed_path=str(installed), installed_sha256=sha(installed),
                scope='Historical OLE guest build and package identity; no source-to-binary or runtime behavior claim')

if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
