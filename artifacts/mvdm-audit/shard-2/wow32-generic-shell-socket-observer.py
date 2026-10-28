"""Read-only generic thunk, shell and Winsock header selection audit."""
import csv, hashlib, json, subprocess
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
B = Path('build/.ninja/msvc-x86-release/wow32').resolve()
NAMES = ('wkgthunk.h','wshtbl2.h','wsocktbl.h')

def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else ''

def inspect():
    frozen={r['relative_path']:r for r in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    deps=subprocess.run(['ninja','-C',str(B),'-t','deps','obj/wowtbl.obj'],check=True,capture_output=True,text=True).stdout
    paths={str((B/line.strip()).resolve()).lower() for line in deps.splitlines() if line.startswith('    ')}
    rows=[]
    for name in NAMES:
        rel='wow32/'+name; f=frozen[rel]; hashes=[sha(root/rel) for root in (M,*O)]
        assert hashes == [f['current_sha256'],f['opennt_sha256'],f['opennt45_sha256']]
        assert str((M/rel).resolve()).lower() in paths
        rows.append(dict(path=rel,hashes=hashes,dos_catalog_rows=f['dos_catalog_rows'],
                         wow_catalog_rows=f['wow_catalog_rows'],normalized_dependency=str((M/rel).resolve())))
    names=('wow32/wktbl2.h','wow32/wshtbl2.h','wow32/wwstbl2.h')
    for name,func in zip(names,('WK32ICallProc32','WS32RegOpenKey','WWS32accept')):
        assert func in (M/name).read_text(encoding='latin1')
    graph=(B/'build.ninja').read_text(encoding='utf-8').replace('$:',':')
    edges=[next(line for line in graph.splitlines() if line.startswith('build obj/'+n+'.obj:')) for n in ('wowtbl','wkgthunk','wshell','wsraw')]
    mp=(B/'wow32.dll.map').read_text(encoding='latin1')
    tokens=('_wow_original_WK32ICallProc32@4','_WK32ICallProc32@4','_WS32RegOpenKey@4','_WWS32accept@4')
    symbols=[next(line.strip() for line in mp.splitlines() if token in line.split()) for token in tokens]
    return dict(result='pass',sources=rows,normalized_deps_object='obj/wowtbl.obj',compile_edges=edges,map_symbols=symbols,
                graph_sha256=sha(B/'build.ninja'),map_sha256=sha(B/'wow32.dll.map'),
                tables=[dict(path=n,sha256=sha(M/n)) for n in names],
                scope='Normalized compile dependencies and representative table/map evidence; not live calls, complete ABI or lifecycle')

if __name__ == '__main__':
    print(json.dumps(inspect(),indent=2))
