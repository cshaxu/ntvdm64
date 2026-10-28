"""Read-only DOS current-directory header and guest include role observations."""
import csv, hashlib, json, re, subprocess
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
WB = Path('build/.ninja/msvc-x86-release/worker').resolve()
WOB = Path('build/.ninja/msvc-x86-release/wow32').resolve()
NAMES = ('inc/curdir.h','inc/CURDIR.INC')

def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else ''

def dep(base,obj,path):
    out=subprocess.run(['ninja','-C',str(base),'-t','deps',obj],check=True,capture_output=True,text=True).stdout
    paths={str((base/line.strip()).resolve()).lower() for line in out.splitlines() if line.startswith('    ')}
    return str(path.resolve()).lower() in paths

def inspect():
    frozen={r['relative_path']:r for r in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    rows=[]
    for rel in NAMES:
        f=frozen[rel]; hashes=[sha(root/rel) for root in (M,*O)]
        assert hashes == [f['current_sha256'],f['opennt_sha256'],f['opennt45_sha256']]
        rows.append(dict(path=rel,hashes=hashes,dos_catalog_rows=f['dos_catalog_rows'],wow_catalog_rows=f['wow_catalog_rows']))
    hdr=(M/NAMES[0]).read_text(encoding='latin1').lower()
    inc=(M/NAMES[1]).read_text(encoding='latin1').lower()
    for name in ('curdir_text','curdir_flags','curdir_end','curdir_tosync'):
        assert name in hdr and name in inc
    assert dep(WB,'obj/dem/demfile.obj',M/NAMES[0]) and dep(WOB,'obj/wdos.obj',M/NAMES[0])
    paths=('inc/makefile','dos/v86/doskrnl/dos/mscode.asm','dos/v86/doskrnl/dos/path.asm','dos/v86/doskrnl/dos/msdos.lnk')
    ctx=[M/n for n in paths]; mk,code,path,link=[p.read_text(encoding='latin1').lower() for p in ctx]
    assert 'curdir.inc:   curdir.h bop.h' in mk and 'include curdir.inc' in code and 'include curdir.inc' in path
    assert 'mscode.obj' in link and 'path.obj' in link
    mp=(WOB/'wow32.dll.map').read_text(encoding='latin1')
    sym=next(line.strip() for line in mp.splitlines() if '_GetCurrentDir@8' in line.split() and 'wdos.obj' in line)
    return dict(result='pass',sources=rows,context=[dict(path=str(p),sha256=sha(p)) for p in ctx],
                normalized_dependencies=[dict(build=str(WB),object='obj/dem/demfile.obj'),dict(build=str(WOB),object='obj/wdos.obj')],
                wow_map_entry=sym,worker_graph_sha256=sha(WB/'build.ninja'),wow_map_sha256=sha(WOB/'wow32.dll.map'),
                scope='Source field names, normalized host dependencies and historical guest build references, not layout or behavior proof')

if __name__ == '__main__':
    print(json.dumps(inspect(),indent=2))
