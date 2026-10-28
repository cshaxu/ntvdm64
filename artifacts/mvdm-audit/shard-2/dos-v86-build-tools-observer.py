"""Read-only original DOS guest-build tool inputs and consuming recipes."""
import csv, hashlib, json
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
ROOT = 'dos/v86/tools/src/'
NAMES = ('getsize/getsize.def','h2inc/h2inc.c','stripdd/stripdd.c','stripz/stripz.c')

def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else ''

def inspect():
    frozen = {r['relative_path']: r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    rows = []
    for name in NAMES:
        rel = ROOT + name; f = frozen[rel]
        hashes = [sha(root / rel) for root in (M, *O)]
        assert hashes == [f['current_sha256'], f['opennt_sha256'], f['opennt45_sha256']]
        assert hashes[0] and hashes[0] in hashes[1:]
        rows.append(dict(path=rel, hashes=hashes, dos_catalog_rows=f['dos_catalog_rows'], wow_catalog_rows=f['wow_catalog_rows']))
    names = (ROOT+'stripdd/sources', ROOT+'stripz/sources', ROOT+'getsize/sources', ROOT+'h2inc/h2inc.mak',
             'dos/v86/dev/himem/makefile', 'dos/v86/doskrnl/dos/makefile', 'dos/v86/redir/makefile',
             'dos/v86/doskrnl/dos/makedos.cmd')
    ctx = [M / n for n in names]
    assert all(sha(p) == sha(O[0] / p.relative_to(M)) for p in ctx)
    dd, z, size, h2, himem, dos, redir, cmd = [p.read_text(encoding='latin1').lower() for p in ctx]
    assert 'sources=stripdd.c' in dd and 'buildtool=1' in dd and 'sources=stripz.c' in z and 'buildtool=1' in z
    assert 'sources=getsize.c' in size and 'buildtool=1' in size
    assert 'h2inc.obj:' in h2 and 'h2inc.c' in h2 and 'h2inc.exe:' in h2
    assert 'stripdd himem.bin $(dest)' in himem and 'stripz ntdos.bin $(dest)' in dos
    assert 'h2inc -c $< -o $*.inc' in redir and 'getsize' in cmd
    return dict(result='pass', sources=rows, context=[dict(path=str(p), sha256=sha(p)) for p in ctx],
                scope='Historical build tool source and consumer recipes; no executable tool generation or guest runtime claim')

if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
