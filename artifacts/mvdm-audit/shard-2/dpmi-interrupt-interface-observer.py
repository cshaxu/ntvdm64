"""Read-only original interrupt declarations and selected DPMI host object."""
import csv, hashlib, json, subprocess
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
B = Path('build/.ninja/msvc-x86-release/worker').resolve()
NAMES = ('dpmi32/precomp.h','inc/intapi.h','inc/VINT.INC')

def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else ''

def inspect():
    frozen={r['relative_path']:r for r in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    out=subprocess.run(['ninja','-C',str(B),'-t','deps','obj/dpmi/dpmiint.obj'],check=True,capture_output=True,text=True).stdout
    deps={str((B/line.strip()).resolve()).lower() for line in out.splitlines() if line.startswith('    ')}
    rows=[]
    for rel in NAMES:
        f=frozen[rel]; hashes=[sha(root/rel) for root in (M,*O)]
        assert hashes == [f['current_sha256'],f['opennt_sha256'],f['opennt45_sha256']]
        selected=str((M/rel).resolve()).lower() in deps
        if rel != 'inc/VINT.INC': assert selected
        rows.append(dict(path=rel,hashes=hashes,dos_catalog_rows=f['dos_catalog_rows'],wow_catalog_rows=f['wow_catalog_rows'],
                         dpmiint_normalized_dependency=selected))
    names=('dpmi32/dpmiint.c','dos/v86/dev/himem/himem4.asm','wow32/i386/fastwow.asm','inc/makefile')
    ctx=[M/n for n in names]; impl,guest,fast,make=[p.read_text(encoding='latin1').lower() for p in ctx]
    assert '#include "precomp.h"' in impl and '#include <intapi.h>' in impl
    assert 'vdminstallhardwareinthandler' in impl and 'include vint.inc' in guest and 'include vint.inc' in fast
    assert 'vint.inc:   vint.h' in make
    graph=(B/'build.ninja').read_text(encoding='utf-8').replace('$:',':')
    edge=next(line for line in graph.splitlines() if line.startswith('build obj/dpmi/dpmiint.obj:'))
    lib=next(line for line in graph.splitlines() if line.startswith('build original-mvdm-dpmi32.lib:'))
    assert 'obj/dpmi/dpmiint.obj' in lib.split()
    mp=(B/'ntvdm.exe.map').read_text(encoding='latin1')
    sym=next(line.strip() for line in mp.splitlines() if '_DpmiSetProtectedmodeInterrupt' in line.split())
    return dict(result='pass',sources=rows,context=[dict(path=str(p),sha256=sha(p)) for p in ctx],
                normalized_deps_object='obj/dpmi/dpmiint.obj',compile_edge=edge,map_entry=sym,
                graph_sha256=sha(B/'build.ninja'),map_sha256=sha(B/'ntvdm.exe.map'),
                scope='Interrupt declaration selection and source references; no vector ownership, reentry or guest ABI proof')

if __name__ == '__main__':
    print(json.dumps(inspect(),indent=2))
