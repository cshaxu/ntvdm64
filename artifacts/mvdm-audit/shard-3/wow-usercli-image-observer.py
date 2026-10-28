"""Observe original USER client thunks and released dispatch slots."""
import csv
import hashlib
import json
import runpy
import struct
import subprocess
from pathlib import Path

A=Path('artifacts/mvdm-audit')
B=Path('build/.ninja/msvc-x86-release/wow32')


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    h=runpy.run_path(str(A/'host-tail-image-observer.py'))
    frozen={r['relative_path']:r for r in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    graph=(B/'build.ninja').read_text().replace('$:',':')
    link=next(s for s in graph.splitlines() if s.startswith('build wow32.dll |'))
    assert 'obj/wusercli.obj' in link.split() and 'obj/wowtbl.obj' in link.split()
    deps=subprocess.check_output(['ninja','-C',str(B),'-t','deps','obj/wusercli.obj','obj/wowtbl.obj'],text=True)
    assert deps.count('(VALID)')==2 and '(STALE)' not in deps and 'wusercli.h' in deps.lower()
    release=Path('assets/release/WOW32.DLL');image=h['Image'](release);cached=h['Image'](B/'wow32.dll')
    assert image.base==cached.base and image.code==cached.code
    assert all(image.data[s:e]==cached.data[s:e] for s,e,_ in image.code)
    symbols=h['symbols']((B/'wow32.dll.map').read_text())
    obj=h['Coff'](B/'obj/wusercli.obj')
    mapped=[s for s in symbols if s['function'] and s['owner'].lower()=='wusercli.obj']
    assert len(mapped)==30
    functions=[h['bind'](image,obj,s) for s in mapped]
    sources=[];inputs=[release,B/'wow32.dll',B/'wow32.dll.map',B/'build.ninja',B/'obj/wusercli.obj',B/'obj/wowtbl.obj']
    for name in ('wusercli.c','wusercli.h'):
        relative='wow32/'+name;record=frozen[relative]
        source=Path('src/mvdm')/relative;first=Path('O:/repos.external/OpenNT/base/mvdm')/relative
        second=Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')/relative
        assert sha(source)==sha(first)==sha(second)==record['current_sha256']==record['opennt_sha256']==record['opennt45_sha256']
        assert record['dos_catalog_rows']=='1' and record['wow_catalog_rows']=='2'
        sources.append(dict(relative_path=relative,sha256=sha(source),original_path=str(first),wow_source_ids=record['wow_source_ids']))
        inputs.extend([source,first,second])
    tbl=h['Coff'](B/'obj/wowtbl.obj')
    definition=[s for s in tbl.symbols.values() if s[0]=='_aw32WOW' and s[2]>0]
    assert len(definition)==1
    _,start,section=definition[0];body,relocs=tbl.sections[section-1]
    table=next(s for s in symbols if s['name']=='_aw32WOW' and s['owner'].lower()=='wowtbl.obj')
    targets={s['name']:s for s in mapped}
    selected=[(p,n,k) for p,n,k in relocs if n in targets and k==6]
    assert len(selected)==32
    slots=[]
    for pos,name,kind in selected:
        assert pos>=start and (pos-start)%4==0
        index=(pos-start)//4; ordinal=index-311
        assert ordinal>=0
        actual=struct.unpack('<I',h['data_at'](image,table['va']+pos-start,4))[0]
        assert actual==(targets[name]['va']+struct.unpack_from('<I',body,pos)[0])&0xffffffff
        slots.append(dict(user_ordinal=ordinal,table_index=index,symbol=name,va=actual))
    assert len({s['symbol'] for s in slots})==30
    return dict(result='pass',sources=sources,functions=functions,user_slots=slots,
        inputs=[dict(path=str(p),sha256=sha(p)) for p in inputs],
        scope='Selected host thunk code and initial pointers; immutable guest optimized local paths and runtime behavior unproven')


if __name__=='__main__':
    print(json.dumps(inspect(),indent=2))
