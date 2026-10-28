"""Read-only historical DPMI DOSX build profile and image identities."""
import csv, hashlib, json, re, zipfile
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
ROOT = 'dpmi/'
NAMES = ('486/dxintr.asm','dosx.def','dxhpbios.asm','dxini.asm','dxmmgr.asm','dxnetbio.asm',
         'dxoem.asm','dxstrt.asm','migrate.bat','mk.cmd','ntnpxem.asm','usa/dxmsg.asm')

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
    names = ('makefile','dosx.lnk','dosxi.lnk')
    ctx = [M / (ROOT+n) for n in names]
    assert all(sha(p) == sha(O[0] / p.relative_to(M)) for p in ctx)
    make, generic, x86 = [p.read_text(encoding='latin1').lower() for p in ctx]
    assert 'lnkfile = dosxi.lnk' in make and 'lnkfile = dosx.lnk' in make
    for token in ('dxintr.obj','dxini.obj','dxmmgr.obj','dxnetbio.obj','dxoem.obj','dxmsg.obj'):
        assert token in generic and token in x86 and token in make
    assert 'ntnpxem.obj' in x86 and 'ntnpxem.obj' not in generic and '486\\dxstrt.asm' in make
    assert 'country=usa' in make and 'dxmsg.asm: $(country)\\dxmsg.asm' in make
    assert 'dxhpbios.obj dxhpbios.lst:' in make and 'dxhpbios.obj' not in x86
    archive = Path('assets/winnt.zip')
    with zipfile.ZipFile(archive) as z:
        matches = [dict(member=n, sha256=digest(z.read(n))) for n in z.namelist() if Path(n).name.lower() == 'dosx.exe']
    source = M / 'dpmi/486/dosx.exe'; installed = Path('O:/winnt/system32/DOSX.EXE')
    return dict(result='pass', sources=rows, context=[dict(path=str(p), sha256=sha(p)) for p in ctx],
                archive_sha256=sha(archive), archive_basename_matches=matches, source_image_sha256=sha(source),
                installed_path=str(installed), installed_sha256=sha(installed),
                scope='Historical DPMI profile and image identity; no deployed guest execution or source-to-image claim')

if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
