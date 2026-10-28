"""Read-only disk legacy-provider guard and header selection evidence."""
import csv, hashlib, json, re, struct, subprocess
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
B = Path('build/.ninja/msvc-x86-release/worker').resolve()
NAMES=('softpc.new/base/disks/gfi_sflp.c','softpc.new/base/inc/cdrom.h')

def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else ''

def inspect():
    frozen={r['relative_path']:r for r in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    out=subprocess.run(['ninja','-C',str(B),'-t','deps','obj/disks/gfi_sflp.obj'],check=True,capture_output=True,text=True).stdout
    deps={str((B/line.strip()).resolve()).lower() for line in out.splitlines() if line.startswith('    ')}
    rows=[]
    for rel in NAMES:
        f=frozen[rel]; hashes=[sha(root/rel) for root in (M,*O)]
        assert hashes == [f['current_sha256'],f['opennt_sha256'],f['opennt45_sha256']]
        rows.append(dict(path=rel,hashes=hashes,dos_catalog_rows=f['dos_catalog_rows'],wow_catalog_rows=f['wow_catalog_rows'],
                         gfi_sflp_normalized_dependency=str((M/rel).resolve()).lower() in deps))
    source=(M/NAMES[0]).read_text(encoding='latin1')
    assert '#ifdef SLAVEPC' in source
    graph=(B/'build.ninja').read_text(encoding='utf-8').replace('$:',':')
    flags=next(line for line in graph.splitlines() if line.startswith('cflags ='))
    assert not re.search(r'/DSLAVEPC(?:\s|=|$)',flags,re.I)
    edge=next(line for line in graph.splitlines() if line.startswith('build obj/disks/gfi_sflp.obj:'))
    lib=next(line for line in graph.splitlines() if line.startswith('build original-softpc-disks.lib:'))
    assert 'obj/disks/gfi_sflp.obj' in lib.split()
    obj=B/'obj/disks/gfi_sflp.obj'; raw=obj.read_bytes()
    machine,count,_,_,_,optional,_=struct.unpack_from('<HHIIIHH',raw,0)
    assert machine==0x14c and optional==0
    sections=[]
    for i in range(count):
        off=20+i*40; flag=struct.unpack_from('<I',raw,off+36)[0]
        sections.append(dict(name=raw[off:off+8].rstrip(b'\0').decode('ascii'),flags=flag))
        assert not flag & 0x20
    mp=(B/'ntvdm.exe.map').read_text(encoding='latin1')
    assert not re.search(r'(?im)^.*\bgfi_sflp\.obj\s*$',mp)
    return dict(result='pass',sources=rows,compile_edge=edge,object_sha256=sha(obj),sections=sections,
                graph_sha256=sha(B/'build.ninja'),map_sha256=sha(B/'ntvdm.exe.map'),
                scope='Guarded remote floppy provider and single-object dependency snapshot, not feature execution or total header-use proof')

if __name__ == '__main__':
    print(json.dumps(inspect(),indent=2))
