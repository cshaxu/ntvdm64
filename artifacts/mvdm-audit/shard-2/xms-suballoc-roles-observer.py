"""Read-only selected XMS dispatch and suballocator build-role observations."""
import csv, hashlib, json, subprocess
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
B = Path('build/.ninja/msvc-x86-release/worker').resolve()
NAMES = ('xms.486/xmsdisp.c','suballoc/suballoc.c','suballoc/suballoc.txt','suballoc/foo.bar')

def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else ''

def inspect():
    frozen={r['relative_path']:r for r in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    deps=subprocess.run(['ninja','-C',str(B),'-t','deps','obj/suballoc/suballoc.obj'],check=True,capture_output=True,text=True).stdout
    paths={str((B/line.strip()).resolve()).lower() for line in deps.splitlines() if line.startswith('    ')}
    rows=[]
    for rel in NAMES:
        f=frozen[rel]; hashes=[sha(root/rel) for root in (M,*O)]
        assert hashes == [f['current_sha256'],f['opennt_sha256'],f['opennt45_sha256']]
        rows.append(dict(path=rel,hashes=hashes,dos_catalog_rows=f['dos_catalog_rows'],
                         wow_catalog_rows=f['wow_catalog_rows'],suballoc_normalized_dependency=str((M/rel).resolve()).lower() in paths))
    source=(M/'xms.486/xmsdisp.c').read_text(encoding='latin1').lower()
    assert 'xmsallocblock' in source and 'apfnxmssvc [isvc]' in source and '#if dbg' in source
    assert 'suballcp.h' in (M/'suballoc/foo.bar').read_text(encoding='latin1').lower()
    assert 'this module contains code' in (M/'suballoc/suballoc.txt').read_text(encoding='latin1').lower()
    graph=(B/'build.ninja').read_text(encoding='utf-8').replace('$:',':')
    needles=('build obj/xms/xmsdisp.obj:','build obj/suballoc/suballoc.obj:',
             'build original-mvdm-xms.lib:','build original-mvdm-host-suballoc.lib:')
    edges=[next(line for line in graph.splitlines() if line.startswith(n)) for n in needles]
    assert 'obj/xms/xmsdisp.obj' in edges[2] and 'obj/suballoc/suballoc.obj' in edges[3]
    mp=(B/'ntvdm.exe.map').read_text(encoding='latin1')
    symbols=[next(line.strip() for line in mp.splitlines() if token in line.split()) for token in ('_XMSDispatch','_SAAllocate','_SAInitialize')]
    return dict(result='pass',sources=rows,normalized_deps_object='obj/suballoc/suballoc.obj',compile_and_library_edges=edges,
                map_symbols=symbols,graph_sha256=sha(B/'build.ninja'),map_sha256=sha(B/'ntvdm.exe.map'),
                scope='Historical source identity and selected link snapshot; no runtime input bounds or allocator lifecycle proof')

if __name__ == '__main__':
    print(json.dumps(inspect(),indent=2))
