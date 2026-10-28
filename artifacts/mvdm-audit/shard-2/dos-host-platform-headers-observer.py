"""Read-only host platform header identity and normalized worker dependency scan."""
import csv, hashlib, json, re, subprocess
from pathlib import Path

A,M=Path('artifacts/mvdm-audit'),Path('src/mvdm')
O=(Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
B=Path('build/.ninja/msvc-x86-release/worker').resolve()
NAMES=('softpc.new/base/inc/host_msw.h','softpc.new/base/inc/ms_windo.h','softpc.new/base/inc/winfiles.h',
       'softpc.new/host/inc/host.h','softpc.new/host/inc/host_inc.h')

def sha(p): return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else ''

def inspect():
    frozen={r['relative_path']:r for r in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    raw=subprocess.run(['ninja','-C',str(B),'-t','deps'],check=True,capture_output=True,text=True).stdout
    lookup={str((M/n).resolve()).lower():n for n in NAMES}; hits={n:[] for n in NAMES}; obj=''; valid=False
    for line in raw.splitlines():
        if line and not line[0].isspace() and ': #deps ' in line:
            obj=line.split(': #deps ',1)[0]; valid='(VALID)' in line
        elif line.startswith('    ') and valid:
            name=lookup.get(str((B/line.strip()).resolve()).lower())
            if name is not None and obj not in hits[name]: hits[name].append(obj)
    rows=[]
    for rel in NAMES:
        f=frozen[rel]; hashes=[sha(root/rel) for root in (M,*O)]
        assert hashes == [f['current_sha256'],f['opennt_sha256'],f['opennt45_sha256']]
        rows.append(dict(path=rel,hashes=hashes,dos_catalog_rows=f['dos_catalog_rows'],wow_catalog_rows=f['wow_catalog_rows'],
                         valid_deps_count=len(hits[rel]),valid_deps_objects=hits[rel]))
    assert 'obj/cvidc/accessfn.obj' in hits[NAMES[4]]
    bios=(M/'softpc.new/base/bios/bios.c').read_text(encoding='latin1')
    assert re.search(r'#ifdef\s+SWIN_HFX\s*\n\s*#include\s+"winfiles.h"',bios)
    graph=(B/'build.ninja').read_text(encoding='utf-8').replace('$:',':')
    edge=next(line for line in graph.splitlines() if line.startswith('build obj/cvidc/accessfn.obj:'))
    return dict(result='pass',sources=rows,example_compile_edge=edge,bios_source_sha256=sha(M/'softpc.new/base/bios/bios.c'),
                graph_sha256=sha(B/'build.ninja'),scope='Exact normalized VALID dependency snapshot and conditional source references, not all-profile/runtime proof')

if __name__ == '__main__': print(json.dumps(inspect(),indent=2))
