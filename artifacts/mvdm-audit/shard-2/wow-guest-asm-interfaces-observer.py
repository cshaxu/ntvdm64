"""Read-only original WOW guest assembly interfaces and selected-object dependencies."""
import csv, hashlib, json, subprocess
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
B = Path('build/.ninja/msvc-x86-release/wow32').resolve()
NAMES = ('WOW.INC','WOWMMCB.INC','WOWNT32.INC','WOWTH.INC')

def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else ''

def inspect():
    frozen={r['relative_path']:r for r in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    deps=subprocess.run(['ninja','-C',str(B),'-t','deps','obj/wowtbl.obj'],check=True,capture_output=True,text=True).stdout
    paths={str((B/line.strip()).resolve()).lower() for line in deps.splitlines() if line.startswith('    ')}
    rows=[]
    for name in NAMES:
        rel='inc/'+name; f=frozen[rel]; hashes=[sha(root/rel) for root in (M,*O)]
        assert hashes == [f['current_sha256'],f['opennt_sha256'],f['opennt45_sha256']]
        rows.append(dict(path=rel,hashes=hashes,dos_catalog_rows=f['dos_catalog_rows'],wow_catalog_rows=f['wow_catalog_rows'],
                         wowtbl_normalized_dependency=str((M/rel).resolve()).lower() in paths))
    names=('inc/makefile','wow16/gdi/gdi.asm','wow16/toolhelp/ththunks.asm','wow16/toolhelp/makefile',
           'wow16/mmsystem/stack.asm','wow16/mmsystem/isr.asm','wow16/mmsystem/makefile')
    ctx=[M/n for n in names]
    mk,gdi,th,thmake,stack,isr,mm=[p.read_text(encoding='latin1').lower() for p in ctx]
    assert 'wow.inc:      wow.h mvdm.h bop.h' in mk and 'wownt32.inc:' in mk and 'wowth.inc:' in mk
    assert 'include wow.inc' in gdi and 'include wowth.inc' in th and 'ththunks.obj:' in thmake
    assert 'include wowmmcb.inc' in stack and 'include wowmmcb.inc' in isr
    assert '$zstack.obj' in mm and '$zisr.obj' in mm
    graph=(B/'build.ninja').read_text(encoding='utf-8').replace('$:',':')
    edge=next(line for line in graph.splitlines() if line.startswith('build obj/wowtbl.obj:'))
    return dict(result='pass',sources=rows,context=[dict(path=str(p),sha256=sha(p)) for p in ctx],
                normalized_deps_object='obj/wowtbl.obj',compile_edge=edge,graph_sha256=sha(B/'build.ninja'),
                scope='Historical guest assembly references and one host dependency snapshot, not complete build closure or guest ABI proof')

if __name__ == '__main__':
    print(json.dumps(inspect(),indent=2))
