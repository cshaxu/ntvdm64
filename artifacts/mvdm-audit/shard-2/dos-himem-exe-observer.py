"""Inspect HIMEM intermediate and XMS dispatch without executing or rebuilding guest."""
import csv, hashlib, json, re, zipfile
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
REL = 'dos/v86/dev/himem/himem.exe'

def digest(data):
    return hashlib.sha256(data).hexdigest().upper()

def sha(p):
    return digest(p.read_bytes()) if p.is_file() else ''

def inspect():
    frozen = {r['relative_path']: r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    f = frozen[REL]; hashes = [sha(root / REL) for root in (M, *O)]
    assert hashes == [f['current_sha256'], f['opennt_sha256'], f['opennt45_sha256']]
    names = ('dos/v86/dev/himem/makefile', 'dos/v86/dev/himem/himem4.asm', 'dos/v86/dev/himem/himem2.asm', 'xms.486/xmsdisp.c')
    ctx = [M / n for n in names]
    assert all(sha(p) == sha(O[0] / p.relative_to(M)) for p in ctx)
    make, guest, init, dispatch = [p.read_text(encoding='latin1').lower() for p in ctx]
    assert 'himem.exe: himem.obj' in make and 'himem.bin: himem.exe' in make and 'stripdd himem.bin $(dest)' in make
    assert re.search(r'xmssvc\s+xms_allocblock', guest) and 'xmsallocblock' in dispatch
    assert 'ifdef\tdebug_tsr' in init and 'exestart' in init
    archive = Path('assets/winnt.zip')
    with zipfile.ZipFile(archive) as z:
        members = [n for n in z.namelist() if n.lower() == 'winnt/system32/himem.sys']
        assert len(members) == 1
        image = digest(z.read(members[0]))
        exe_members = [n for n in z.namelist() if Path(n).name.lower() == 'himem.exe']
    source_sys = M / 'dos/v86/dev/himem/HIMEM.SYS'
    installed = Path('O:/winnt/system32/HIMEM.SYS')
    assert image == sha(source_sys) == sha(installed)
    return dict(result='pass', relative_path=REL, hashes=hashes, dos_catalog_rows=f['dos_catalog_rows'],
                wow_catalog_rows=f['wow_catalog_rows'], context=[dict(path=str(p), sha256=sha(p)) for p in ctx],
                archive_sha256=sha(archive), sys_member=members[0], sys_sha256=image,
                archive_himem_exe_members=exe_members, installed_sys_sha256=sha(installed),
                scope='Historical intermediate recipe and XMS source dispatch, not image provenance or runtime')

if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
