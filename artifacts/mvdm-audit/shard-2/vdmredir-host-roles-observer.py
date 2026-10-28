"""Read-only redirector selected host input, normalized deps and DLL identity."""
import csv, hashlib, json, subprocess
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
B = Path('build/.ninja/msvc-x86-release/worker').resolve()
NAMES = ('dlstruct.h','vdmredir.rc','vrdisp.c','vrica.h','vrputil.h')

def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else ''

def deps(obj):
    out=subprocess.run(['ninja','-C',str(B),'-t','deps',obj],check=True,capture_output=True,text=True).stdout
    return {str((B/line.strip()).resolve()).lower() for line in out.splitlines() if line.startswith('    ')}

def inspect():
    frozen={r['relative_path']:r for r in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    rows=[]
    for name in NAMES:
        rel='vdmredir/'+name; f=frozen[rel]; hashes=[sha(root/rel) for root in (M,*O)]
        assert hashes == [f['current_sha256'],f['opennt_sha256'],f['opennt45_sha256']]
        rows.append(dict(path=rel,hashes=hashes,dos_catalog_rows=f['dos_catalog_rows'],wow_catalog_rows=f['wow_catalog_rows']))
    checks=(('obj/redir/vrnetapi.obj','dlstruct.h'),('obj/redir/vrnetapi.obj','vrputil.h'),('obj/redir/vrnetb.obj','vrica.h'))
    normalized=[]
    for obj,name in checks:
        path=(M/('vdmredir/'+name)).resolve(); assert str(path).lower() in deps(obj)
        normalized.append(dict(object=obj,path=str(path)))
    graph=(B/'build.ninja').read_text(encoding='utf-8').replace('$:',':')
    needles=('build obj/redir/vrdisp.obj:','build obj/redir/vdmredir.res:', 'build VDMREDIR.dll | VDMREDIR.dll.lib:')
    edges=[next(line for line in graph.splitlines() if line.startswith(n)) for n in needles]
    assert 'obj/redir/vrdisp.obj' in edges[2].split() and 'obj/redir/vdmredir.res' in edges[2].split()
    disp=(M/'vdmredir/vrdisp.c').read_text(encoding='latin1')
    assert 'VrDispatchTable' in disp and 'VrDlc5cHandler' in disp and 'VrNetbios5c' in disp
    rc=(M/'vdmredir/vdmredir.rc').read_text(encoding='latin1').lower()
    assert 'vdmredir.dll' in rc and '#include "common.ver"' in rc
    images=(B/'VDMREDIR.dll',Path('assets/release/VDMREDIR.DLL'),Path('O:/winnt/system32/VDMREDIR.DLL'))
    return dict(result='pass',sources=rows,normalized_dependencies=normalized,compile_resource_edges=edges[:2],
                link_has_dispatch_and_resource=True,graph_sha256=sha(B/'build.ninja'),
                images=[dict(path=str(p),sha256=sha(p)) for p in images],
                scope='Current redirector build input and identity snapshot; no live service, remote partner, interrupt or lifecycle proof')

if __name__ == '__main__':
    print(json.dumps(inspect(),indent=2))
