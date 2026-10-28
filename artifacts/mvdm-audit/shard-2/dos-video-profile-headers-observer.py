"""Read-only selected EGA header and alternate video profile declarations."""
import csv, hashlib, json, re, subprocess
from pathlib import Path

A, M=Path('artifacts/mvdm-audit'),Path('src/mvdm')
O=(Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
B=Path('build/.ninja/msvc-x86-release/worker').resolve()
NAMES=('softpc.new/base/inc/gisp_sas.h','softpc.new/host/inc/hostgrph.h','softpc.new/host/inc/nt_ega.h')

def sha(p): return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else ''

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
    assert dep('obj/host/nt_ega.obj',NAMES[2])
    reset=(M/'softpc.new/base/bios/reset.c').read_text(encoding='latin1')
    assert re.search(r'#ifdef\s+GISP_SVGA\s*\n\s*#include\s+"gisp_sas.h"',reset)
    graph=(B/'build.ninja').read_text(encoding='utf-8').replace('$:',':')
    flags=next(line for line in graph.splitlines() if line.startswith('cflags ='))
    assert not re.search(r'/DGISP_SVGA(?:\s|=|$)',flags,re.I)
    edge=next(line for line in graph.splitlines() if line.startswith('build obj/host/nt_ega.obj:'))
    mp=(B/'ntvdm.exe.map').read_text(encoding='latin1')
    names=('_nt_ega_mono_hi_graph_big','_nt_change_mode')
    symbols=[next(line.strip() for line in mp.splitlines() if name in line.split()) for name in names]
    return dict(result='pass',sources=rows,normalized_nt_ega_dependency=str((M/NAMES[2]).resolve()),
                gisp_in_reset_normalized_dependency=dep('obj/bios/reset.obj',NAMES[0]),compile_edge=edge,map_symbols=symbols,
                graph_sha256=sha(B/'build.ninja'),map_sha256=sha(B/'ntvdm.exe.map'),
                scope='Video interface and selected map snapshot; not proof any mode handler renders or alternate profiles are unused')

if __name__ == '__main__': print(json.dumps(inspect(),indent=2))
