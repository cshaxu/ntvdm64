"""Read-only selected EMS and Sim32 host declaration and build evidence."""
import csv, hashlib, json, subprocess
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
B = Path('build/.ninja/msvc-x86-release/worker').resolve()
NAMES=('softpc.new/host/inc/host_emm.h','softpc.new/host/inc/sim32.h')

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
    assert dep('obj/dos/emm_mngr.obj',NAMES[0]) and dep('obj/host/sim32.obj',NAMES[1])
    emm=(M/'softpc.new/base/dos/emm_mngr.c').read_text(encoding='latin1').lower()
    sim=(M/'softpc.new/host/src/sim32.c').read_text(encoding='latin1').lower()
    hdr=(M/NAMES[1]).read_text(encoding='latin1').lower()
    assert '#include "host_emm.h"' in emm and '#include "sim32.h"' in sim
    assert '#ifdef monitor' in hdr and 'sim32flushvdmpointer' in hdr
    graph=(B/'build.ninja').read_text(encoding='utf-8').replace('$:',':')
    edges=[next(line for line in graph.splitlines() if line.startswith('build '+n+':')) for n in ('obj/dos/emm_mngr.obj','obj/host/sim32.obj')]
    mp=(B/'ntvdm.exe.map').read_text(encoding='latin1')
    symbols=[next(line.strip() for line in mp.splitlines() if token in line.split()) for token in ('_Sim32FlushVDMPointer','_host_allocate_storage')]
    return dict(result='pass',sources=rows,normalized_dependencies=[dict(object='obj/dos/emm_mngr.obj',path=str((M/NAMES[0]).resolve())),
                dict(object='obj/host/sim32.obj',path=str((M/NAMES[1]).resolve()))],compile_edges=edges,map_symbols=symbols,
                graph_sha256=sha(B/'build.ninja'),map_sha256=sha(B/'ntvdm.exe.map'),
                scope='Selected memory headers and representative map symbols; no guest memory lifetime or failure behavior proof')

if __name__ == '__main__':
    print(json.dumps(inspect(),indent=2))
