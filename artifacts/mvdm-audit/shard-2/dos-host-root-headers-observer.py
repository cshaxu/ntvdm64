"""Read-only host BOP/reset/thread header selection in the DOS worker."""
import csv, hashlib, json, subprocess
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
B = Path('build/.ninja/msvc-x86-release/worker').resolve()
ROOT='softpc.new/host/inc/'
NAMES=('host_bop.h','nt_reset.h','nt_thred.h')

def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else ''

def inspect():
    frozen={r['relative_path']:r for r in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    out=subprocess.run(['ninja','-C',str(B),'-t','deps','obj/host/nt_bop.obj'],check=True,capture_output=True,text=True).stdout
    deps={str((B/line.strip()).resolve()).lower() for line in out.splitlines() if line.startswith('    ')}
    rows=[]
    for name in NAMES:
        rel=ROOT+name; f=frozen[rel]; hashes=[sha(root/rel) for root in (M,*O)]
        assert hashes == [f['current_sha256'],f['opennt_sha256'],f['opennt45_sha256']]
        path=(M/rel).resolve(); assert str(path).lower() in deps
        rows.append(dict(path=rel,hashes=hashes,dos_catalog_rows=f['dos_catalog_rows'],wow_catalog_rows=f['wow_catalog_rows'],normalized_dependency=str(path)))
    context=(M/'softpc.new/host/src/nt_bop.c').read_text(encoding='latin1').lower()
    for name in NAMES: assert name in context
    graph=(B/'build.ninja').read_text(encoding='utf-8').replace('$:',':')
    edges=[next(line for line in graph.splitlines() if line.startswith('build obj/host/'+n+'.obj:')) for n in ('nt_bop','nt_reset')]
    mp=(B/'ntvdm.exe.map').read_text(encoding='latin1')
    symbols=[next(line.strip() for line in mp.splitlines() if token in line.split()) for token in ('_MS_bop_2','_MS_bop_4','_host_reset')]
    return dict(result='pass',sources=rows,normalized_deps_object='obj/host/nt_bop.obj',compile_edges=edges,map_symbols=symbols,
                nt_bop_current_sha256=sha(M/'softpc.new/host/src/nt_bop.c'),
                graph_sha256=sha(B/'build.ninja'),map_sha256=sha(B/'ntvdm.exe.map'),
                scope='Selected header dependency and representative host map symbols, not session/reset/BOP lifecycle correctness')

if __name__ == '__main__':
    print(json.dumps(inspect(),indent=2))
