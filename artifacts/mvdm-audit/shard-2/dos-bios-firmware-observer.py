"""Read-only DOS BIOS guest input and firmware profile identity observations."""
import csv, hashlib, json, zipfile
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
B = Path('build/.ninja/msvc-x86-release/worker')
NAMES=('softpc.new/bios/biostruc.inc','softpc.new/bios/devmark.inc','softpc.new/bios/msgroup.inc',
       'softpc.new/bios/sysimes.asm','softpc.new/data/bios1.rom','softpc.new/roms/bios2.rom','softpc.new/roms/bios4.asm')

def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else ''

def inspect():
    frozen={r['relative_path']:r for r in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    rows=[]
    for rel in NAMES:
        f=frozen[rel]; hashes=[sha(root/rel) for root in (M,*O)]
        assert hashes == [f['current_sha256'],f['opennt_sha256'],f['opennt45_sha256']]
        rows.append(dict(path=rel,hashes=hashes,dos_catalog_rows=f['dos_catalog_rows'],wow_catalog_rows=f['wow_catalog_rows']))
    names=('softpc.new/bios/makefile','softpc.new/bios/msbio.lnk','softpc.new/bios/msinit.asm',
           'softpc.new/bios/sysinit1.asm','softpc.new/bios/mschar.asm','softpc.new/base/system/rom.c')
    ctx=[M/n for n in names]; make,link,msinit,sysinit,mschar,rom=[p.read_text(encoding='latin1').lower() for p in ctx]
    assert 'sysimes.obj' in make and 'sysimes+' in link
    assert 'include biostruc.inc' in msinit and 'include devmark.inc' in sysinit and 'include msgroup.inc' in mschar
    assert 'bios1.rom' in rom and 'bios4.rom' in rom and 'bios2.rom' in rom and 'cpu_40_style' in rom
    selected=M/'softpc.new/roms/bios1.rom'; assert sha(M/NAMES[4])==sha(selected)
    rc=B/'generated/softpc-embedded-roms.rc'; rctext=rc.read_text(encoding='utf-8').lower()
    assert 'softpc_bios1 rcdata' in rctext and 'softpc.new/roms/bios1.rom' in rctext
    archive=Path('assets/winnt.zip')
    with zipfile.ZipFile(archive) as z:
        matches=[dict(member=n,sha256=hashlib.sha256(z.read(n)).hexdigest().upper()) for n in z.namelist() if Path(n).name.lower()=='ntio.sys']
    return dict(result='pass',sources=rows,context=[dict(path=str(p),sha256=sha(p)) for p in ctx],
                selected_bios1_path=str(selected),selected_bios1_sha256=sha(selected),generated_rc_sha256=sha(rc),
                archive_sha256=sha(archive),archive_ntio=matches,installed_ntio_sha256=sha(Path('O:/winnt/system32/NTIO.SYS')),
                scope='Historical NTIO inputs and current firmware resource selection, not guest or embedded-image execution proof')

if __name__ == '__main__':
    print(json.dumps(inspect(),indent=2))
