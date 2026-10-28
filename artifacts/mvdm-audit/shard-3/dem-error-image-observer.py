"""Check original DEM error owner, dispatch slots, and a filesystem caller."""
import csv
import hashlib
import json
import re
import runpy
import struct
import subprocess
from pathlib import Path

A=Path('artifacts/mvdm-audit')
B=Path('build/.ninja/msvc-x86-release/worker')
REL='dos/dem/demerror.c'


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    h=runpy.run_path(str(A/'host-tail-image-observer.py'))
    frozen=next(r for r in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig')) if r['relative_path']==REL)
    source=Path('src/mvdm')/REL
    first=Path('O:/repos.external/OpenNT/base/mvdm')/REL
    second=Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')/REL
    assert sha(source)==sha(first)==sha(second)==frozen['current_sha256']==frozen['opennt_sha256']==frozen['opennt45_sha256']
    assert frozen['dos_catalog_rows']=='1' and frozen['wow_catalog_rows']=='2'
    graph=(B/'build.ninja').read_text().replace('$:',':')
    target='obj/dem/demerror.obj'
    edge=next(line for line in graph.splitlines() if line.startswith('build '+target+':'))
    archive=next(line for line in graph.splitlines() if line.startswith('build original-mvdm-dem.lib:'))
    assert source.resolve().as_posix().lower() in edge.lower() and target in archive.split()
    deps=subprocess.check_output(['ninja','-C',str(B),'-t','deps'],text=True)
    dep=next(line for line in deps.splitlines() if line.startswith(target+':'))
    assert '(VALID)' in dep and '(STALE)' not in dep
    image_path=Path('assets/release/ntvdm.exe')
    image=h['Image'](image_path)
    symbols=h['symbols']((B/'ntvdm.exe.map').read_text())
    mapped=[s for s in symbols if s['function'] and s['owner']=='original-mvdm-dem:demerror.obj']
    assert len(mapped)==5
    obj=h['Coff'](B/target)
    functions=[h['bind'](image,obj,s) for s in mapped]
    dispatch=Path('src/mvdm/dos/dem/demdisp.c')
    initializer=re.search(r'PFNSVC\s+apfnSVC\s*\[\]\s*=\s*\{(.*?)\};',dispatch.read_text(),re.S)[1]
    initializer=re.sub(r'/\*.*?\*/|//[^\r\n]*','',initializer,flags=re.S)
    names=['_'+x.strip() for x in initializer.split(',') if x.strip()]
    table_obj=h['Coff'](B/'obj/dem/demdisp.obj')
    definitions=[s for s in table_obj.symbols.values() if s[0]=='_apfnSVC' and s[2]>0]
    assert len(definitions)==1
    _,start,section=definitions[0]
    body,relocs=table_obj.sections[section-1]
    table=next(s for s in symbols if s['name']=='_apfnSVC' and s['owner']=='original-mvdm-dem:demdisp.obj')
    slots=[]
    for name in ('_demSetHardErrorInfo','_demRetry'):
        index=names.index(name); pos=start+index*4
        assert [(n,k) for p,n,k in relocs if p==pos]==[(name,6)]
        owner=next(s for s in mapped if s['name']==name)
        value=struct.unpack('<I',h['data_at'](image,table['va']+index*4,4))[0]
        assert value==(owner['va']+struct.unpack_from('<I',body,pos)[0])&0xffffffff
        slots.append(dict(index=index,symbol=name,va=value))
    caller=next(s for s in symbols if s['name']=='_demOpen' and s['owner']=='original-mvdm-dem:demfile.obj')
    calltarget=next(s for s in mapped if s['name']=='_demClientError')
    caller_obj=h['Coff'](B/'obj/dem/demfile.obj')
    caller_body,caller_relocs=caller_obj.function('_demOpen')
    offsets=[p for p,n,k in caller_relocs if n=='_demClientError' and k==20]
    assert len(offsets)==4
    calls=[]
    for pos in offsets:
        assert caller_body[pos-1] in (0xe8,0xe9)
        actual=struct.unpack('<i',h['data_at'](image,caller['va']+pos,4))[0]
        assert caller['va']+pos+4+actual==calltarget['va']+struct.unpack_from('<i',caller_body,pos)[0]
        calls.append(dict(caller='_demOpen',target='_demClientError',va=caller['va']+pos-1))
    paths=(source,first,second,dispatch,image_path,B/'build.ninja',B/'ntvdm.exe.map',B/target,
           B/'obj/dem/demdisp.obj',B/'obj/dem/demfile.obj')
    return dict(result='pass',relative_path=REL,source_sha256=sha(source),wow_source_ids=frozen['wow_source_ids'],
        original_path=str(first),edge=edge,dependency_header=dep,functions=functions,slots=slots,calls=calls,
        inputs=[dict(path=str(p),sha256=sha(p)) for p in paths],
        scope='Initial dispatch and four original caller edges; no hard-error or retry behavior acceptance')


if __name__=='__main__':
    print(json.dumps(inspect(),indent=2))
