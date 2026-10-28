"""Read-only original diagnostic files, selected archive objects and worker map."""
import csv, hashlib, json, re, struct
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
B = Path('build/.ninja/msvc-x86-release/worker')
ROOT='softpc.new/base/debug/'
NAMES=('dasm.c','ega_dump.c','markfrom','profile.c','trace.c')

def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else ''

def inspect():
    frozen={r['relative_path']:r for r in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    rows=[]
    for name in NAMES:
        rel=ROOT+name; f=frozen[rel]; hashes=[sha(root/rel) for root in (M,*O)]
        assert hashes == [f['current_sha256'],f['opennt_sha256'],f['opennt45_sha256']]
        rows.append(dict(path=rel,hashes=hashes,dos_catalog_rows=f['dos_catalog_rows'],wow_catalog_rows=f['wow_catalog_rows']))
    graph=(B/'build.ninja').read_text(encoding='utf-8').replace('$:',':')
    lib=next(line for line in graph.splitlines() if line.startswith('build original-softpc-base-trace.lib:'))
    flags=next(line for line in graph.splitlines() if line.startswith('cflags ='))
    assert '/DPROD' in flags
    mp=(B/'ntvdm.exe.map').read_text(encoding='latin1'); objects=[]
    for name in ('dasm','ega_dump','profile','trace'):
        edge=next(line for line in graph.splitlines() if line.startswith('build obj/base-debug/'+name+'.obj:'))
        assert 'obj/base-debug/'+name+'.obj' in lib.split()
        obj=B/('obj/base-debug/'+name+'.obj'); raw=obj.read_bytes()
        machine,count,_,_,_,optional,_=struct.unpack_from('<HHIIIHH',raw,0); assert machine==0x14c and optional==0
        sections=[]
        for i in range(count):
            off=20+i*40; flag=struct.unpack_from('<I',raw,off+36)[0]
            sections.append(dict(name=raw[off:off+8].rstrip(b'\0').decode('ascii'),flags=flag))
        owners=[line.strip() for line in mp.splitlines() if re.search(r'(?i)\b'+name+r'\.obj\s*$',line)]
        objects.append(dict(name=name,compile_edge=edge,sha256=sha(obj),sections=sections,map_owner_lines=owners))
    assert not any(s['flags'] & 0x20 for o in objects if o['name'] in ('dasm','ega_dump') for s in o['sections'])
    assert any('io_verbose' in s for o in objects if o['name']=='trace' for s in o['map_owner_lines'])
    mark=(M/(ROOT+'markfrom')).read_text(encoding='latin1').lower()
    assert 'width=64' in mark and 'height=64' in mark and 'depth=1' in mark
    return dict(result='pass',sources=rows,objects=objects,graph_sha256=sha(B/'build.ninja'),map_sha256=sha(B/'ntvdm.exe.map'),
                scope='Captured PROD diagnostic object and map roles; no debugger behavior or all-configuration reachability proof')

if __name__ == '__main__':
    print(json.dumps(inspect(),indent=2))
