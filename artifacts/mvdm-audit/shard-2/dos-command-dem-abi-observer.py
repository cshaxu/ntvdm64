"""Read-only DOS command/DEM host dependency and BOP source observations."""
import csv, hashlib, json, subprocess
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
B = Path('build/.ninja/msvc-x86-release/worker').resolve()
NAMES = ('inc/cmdsvc.h','dos/dem/dosdef.h','inc/BOP.INC')

def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else ''

def dep(obj,rel):
    out=subprocess.run(['ninja','-C',str(B),'-t','deps',obj],check=True,capture_output=True,text=True).stdout
    paths={str((B/line.strip()).resolve()).lower() for line in out.splitlines() if line.startswith('    ')}
    return str((M/rel).resolve()).lower() in paths

def inspect():
    frozen={r['relative_path']:r for r in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    rows=[]
    for rel in NAMES:
        f=frozen[rel]; hashes=[sha(root/rel) for root in (M,*O)]
        assert hashes == [f['current_sha256'],f['opennt_sha256'],f['opennt45_sha256']]
        rows.append(dict(path=rel,hashes=hashes,dos_catalog_rows=f['dos_catalog_rows'],wow_catalog_rows=f['wow_catalog_rows']))
    assert dep('obj/command/cmddisp.obj','inc/cmdsvc.h')
    assert dep('obj/dem/demfile.obj','dos/dem/dosdef.h')
    files=('dos/command/cmddisp.c','dos/dem/dem.h','inc/CMDSVC.INC','inc/bop.h','softpc.new/host/src/nt_bop.c')
    ctx=[M/n for n in files]; dispatch,dem,inc,bop,host=[p.read_text(encoding='latin1').lower() for p in ctx]
    assert '#include <cmdsvc.h>' in dispatch and '#include "dosdef.h"' in dem
    assert 'include bop.inc' in inc and 'bop     bop_cmd' in inc and 'bop_cmd              0x54' in bop
    assert 'cmddispatch((ulong) command)' in host
    graph=(B/'build.ninja').read_text(encoding='utf-8').replace('$:',':')
    edges=[next(line for line in graph.splitlines() if line.startswith('build '+name+':')) for name in ('obj/command/cmddisp.obj','obj/dem/demfile.obj')]
    mp=(B/'ntvdm.exe.map').read_text(encoding='latin1')
    sym=next(line.strip() for line in mp.splitlines() if '_CmdDispatch' in line.split())
    return dict(result='pass',sources=rows,context=[dict(path=str(p),sha256=sha(p)) for p in ctx],
                normalized_dependencies=[dict(object='obj/command/cmddisp.obj',path=str((M/NAMES[0]).resolve())),
                                         dict(object='obj/dem/demfile.obj',path=str((M/NAMES[1]).resolve()))],
                compile_edges=edges,map_entry=sym,graph_sha256=sha(B/'build.ninja'),map_sha256=sha(B/'ntvdm.exe.map'),
                scope='BOP/command source references and normalized dependency snapshot, not guest ABI/runtime proof')

if __name__ == '__main__':
    print(json.dumps(inspect(),indent=2))
