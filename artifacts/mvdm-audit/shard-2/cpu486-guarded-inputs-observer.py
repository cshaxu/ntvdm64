"""Read-only selected CPU profile and guarded instruction object evidence."""
import csv, hashlib, json, re, struct, subprocess
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
B = Path('build/.ninja/msvc-x86-release/worker').resolve()
ROOT='softpc.new/base/ccpu386/'
NAMES=('cmpxchg.c','xadd.c','wbinvd.h')

def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else ''

def inspect():
    frozen={r['relative_path']:r for r in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    rows=[]
    for name in NAMES:
        rel=ROOT+name; f=frozen[rel]; hashes=[sha(root/rel) for root in (M,*O)]
        assert hashes == [f['current_sha256'],f['opennt_sha256'],f['opennt45_sha256']]
        rows.append(dict(path=rel,hashes=hashes,dos_catalog_rows=f['dos_catalog_rows'],wow_catalog_rows=f['wow_catalog_rows']))
    for name in ('cmpxchg.c','xadd.c','wbinvd.c'):
        assert '#ifdef SPC486' in (M/(ROOT+name)).read_text(encoding='latin1')
    out=subprocess.run(['ninja','-C',str(B),'-t','deps','obj/ccpu/wbinvd.obj'],check=True,capture_output=True,text=True).stdout
    deps={str((B/line.strip()).resolve()).lower() for line in out.splitlines() if line.startswith('    ')}
    assert str((M/(ROOT+'wbinvd.h')).resolve()).lower() in deps
    graph=(B/'build.ninja').read_text(encoding='utf-8').replace('$:',':')
    flags=next(line for line in graph.splitlines() if line.startswith('ccpu_flags ='))
    assert '/DSPC386' in flags and not re.search(r'/D(?:SPC486|CPU_486)(?:\s|=|$)',flags,re.I)
    lib=next(line for line in graph.splitlines() if line.startswith('build original-ccpu386.lib:'))
    objrows=[]
    for name in ('cmpxchg','xadd','wbinvd'):
        assert 'obj/ccpu/'+name+'.obj' in lib.split()
        edge=next(line for line in graph.splitlines() if line.startswith('build obj/ccpu/'+name+'.obj:'))
        obj=B/('obj/ccpu/'+name+'.obj'); raw=obj.read_bytes()
        machine,count,_,_,_,optional,_=struct.unpack_from('<HHIIIHH',raw,0)
        assert machine == 0x14c and optional == 0
        sections=[]
        for i in range(count):
            off=20+i*40; flag=struct.unpack_from('<I',raw,off+36)[0]
            sections.append(dict(name=raw[off:off+8].rstrip(b'\0').decode('ascii'),flags=flag))
            assert not flag & 0x20
        objrows.append(dict(name=name,edge=edge,sha256=sha(obj),sections=sections))
    return dict(result='pass',sources=rows,objects=objrows,normalized_wbinvd_dependency=str((M/(ROOT+'wbinvd.h')).resolve()),
                flags_sha256=hashlib.sha256(flags.encode()).hexdigest().upper(),graph_sha256=sha(B/'build.ninja'),
                decoder_sha256=sha(M/(ROOT+'c_main.c')),
                scope='CPU profile guards and captured object sections; not guest instruction/fault behavior')

if __name__ == '__main__':
    print(json.dumps(inspect(),indent=2))
