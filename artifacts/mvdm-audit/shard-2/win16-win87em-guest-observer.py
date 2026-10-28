"""Read-only Win87em guest source profile and shipped image identities."""
import csv, hashlib, json, zipfile
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
ROOT = 'wow16/win87em/'
NAMES = ('emds.asm', 'emoemwin.t1')

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
    ctx = [M / (ROOT + n) for n in ('makefile','emulator.asm','emoemwin.asm')]
    assert all(sha(p) == sha(O[0] / p.relative_to(M)) for p in ctx)
    make = ctx[0].read_text(encoding='latin1').lower()
    assert '$(objdir)\\emmt.lib: $(objdir)\\emmt.obj $(objdir)\\emds.obj' in make
    assert '$(objdir)\\win87em.dll: $(objdir)\\win87em.obj  $(objdir)\\emoemwin.obj' in make
    assert '$(objdir)\\emoemwin.obj: $(srcdir)\\emoemwin.asm' in make
    assert '$(srcdir)\\emds.asm' in make
    archive = Path('assets/winnt.zip')
    with zipfile.ZipFile(archive) as z:
        matches = [n for n in z.namelist() if n.lower() == 'winnt/system32/win87em.dll']
        assert len(matches) == 1
        image = digest(z.read(matches[0]))
    source = M / 'bin86/i386/win87em.dll'; installed = Path('O:/winnt/system32/WIN87EM.DLL')
    return dict(result='pass', sources=rows, context=[dict(path=str(p), sha256=sha(p)) for p in ctx],
                archive_sha256=sha(archive), archive_member=matches[0], image_sha256=image,
                source_image_sha256=sha(source), installed_path=str(installed), installed_sha256=sha(installed),
                scope='Original emulator build profiles and guest image identity; not source-to-binary or floating-point behavior')

if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
