"""Read-only DPMI debug object gating and active neighbor image binding."""
import csv,hashlib,json,re,runpy,struct
from pathlib import Path

A,M=Path('artifacts/mvdm-audit'),Path('src/mvdm')
O=(Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
B=Path('build/.ninja/msvc-x86-release/worker')
REL='dpmi32/debug.c'

def sha(p): return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()

def inspect():
    h=runpy.run_path(str(A/'host-tail-image-observer.py'))
    frozen=next(r for r in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig')) if r['relative_path']==REL)
    ledger=next(r for r in csv.DictReader((A/'combined-initial-status.csv').open(encoding='utf-8-sig')) if r['relative_path']==REL)
    hashes=[sha(root/REL) for root in (M,*O)]
    assert hashes == [frozen['current_sha256'],frozen['opennt_sha256'],frozen['opennt45_sha256']]
    assert ledger['shard']=='2' and ledger['status']=='pending'
    source=(M/REL).read_text(encoding='latin1')
    assert re.search(r'#if\s+DBG\b',source) and 'DpmiDbgTrace' in source and re.search(r'#endif\s*\Z',source)
    graph=(B/'build.ninja').read_text(encoding='utf-8').replace('$:',':')
    edge=next(x for x in graph.splitlines() if x.startswith('build obj/dpmi/debug.obj:'))
    lib=next(x for x in graph.splitlines() if x.startswith('build original-mvdm-dpmi32.lib:'))
    assert 'obj/dpmi/debug.obj' in lib.split() and 'obj/dpmi/dpmiint.obj' in lib.split()
    obj=B/'obj/dpmi/debug.obj'; raw=obj.read_bytes(); machine,count,_,_,_,optional,_=struct.unpack_from('<HHIIIHH',raw,0)
    assert machine==0x14c and optional==0
    sections=[]
    for i in range(count):
        off=20+i*40; flags=struct.unpack_from('<I',raw,off+36)[0]; assert not flags & 0x20
        sections.append(dict(name=raw[off:off+8].rstrip(b'\0').decode('ascii'),flags=flags))
    mp=(B/'ntvdm.exe.map').read_text(); assert not re.search(r'(?im)^.*\bdpmi32:debug\.obj\s*$',mp)
    image=h['Image'](Path('assets/release/ntvdm.exe')); cached=h['Image'](B/'ntvdm.exe')
    assert image.base==cached.base and image.code==cached.code and all(image.data[s:e]==cached.data[s:e] for s,e,_ in image.code)
    syms=h['symbols'](mp)
    symbol=next(s for s in syms if s['name']=='_DpmiSetProtectedmodeInterrupt' and s['owner'].lower().endswith(':dpmiint.obj') and s['function'])
    binding=h['bind'](image,h['Coff'](B/'obj/dpmi/dpmiint.obj'),symbol)
    return dict(result='pass',source=REL,hashes=hashes,compile_edge=edge,object_sha256=sha(obj),sections=sections,
                active_neighbor_binding=binding,neighbor_source_sha256=sha(M/'dpmi32/dpmiint.c'),
                release_sha256=sha('assets/release/ntvdm.exe'),cached_sha256=sha(B/'ntvdm.exe'),
                map_sha256=sha(B/'ntvdm.exe.map'),graph_sha256=sha(B/'build.ninja'),
                scope='Captured no-code debug object and relocation-masked neighboring function, not complete DPMI or debug-profile behavior')

if __name__ == '__main__': print(json.dumps(inspect(),indent=2))
