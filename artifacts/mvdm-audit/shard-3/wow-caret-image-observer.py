"""Resolve original WOW USER caret thunk slots and adapter calls in release."""
import csv
import hashlib
import json
import re
import runpy
import struct
import subprocess
from pathlib import Path

A=Path('artifacts/mvdm-audit')
B=Path('build/.ninja/msvc-x86-release/wow32')
OLD=Path('build/package-uuid/package-c/system32/WOW32.DLL')


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    h=runpy.run_path(str(A/'host-tail-image-observer.py'))
    frozen={r['relative_path']:r for r in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    graph=(B/'build.ninja').read_text().replace('$:',':')
    link=next(s for s in graph.splitlines() if s.startswith('build wow32.dll |'))
    assert 'obj/wucaret.obj' in link.split() and 'obj/wowtbl.obj' in link.split()
    deps=subprocess.check_output(['ninja','-C',str(B),'-t','deps','obj/wucaret.obj','obj/wowtbl.obj'],text=True)
    assert deps.count('(VALID)')==2 and '(STALE)' not in deps and 'wutbl2.h' in deps.lower()
    release=Path('assets/release/WOW32.DLL')
    image=h['Image'](release);cached=h['Image'](B/'wow32.dll');historic=h['Image'](OLD)
    assert image.base==cached.base==historic.base and image.code==cached.code==historic.code
    assert all(image.data[s:e]==cached.data[s:e]==historic.data[s:e] for s,e,_ in image.code)
    symbols=h['symbols']((B/'wow32.dll.map').read_text())
    obj=h['Coff'](B/'obj/wucaret.obj')
    mapped=[s for s in symbols if s['owner'].lower()=='wucaret.obj' and s['function']]
    assert len(mapped)==8
    functions=[h['bind'](image,obj,s) for s in mapped]
    sources=[];inputs=[release,B/'wow32.dll',OLD,B/'wow32.dll.map',B/'build.ninja',B/'obj/wucaret.obj',B/'obj/wowtbl.obj']
    for filename in ('wucaret.c','wutbl2.h'):
        relative='wow32/'+filename;record=frozen[relative]
        source=Path('src/mvdm')/relative;one=Path('O:/repos.external/OpenNT/base/mvdm')/relative
        two=Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')/relative
        assert sha(source)==sha(one)==sha(two)==record['current_sha256']==record['opennt_sha256']==record['opennt45_sha256']
        assert record['dos_catalog_rows']=='1' and record['wow_catalog_rows']=='2'
        sources.append(dict(relative_path=relative,sha256=sha(source),original_path=str(one),wow_source_ids=record['wow_source_ids']))
        inputs.extend([source,one,two])
    names={163:'_WU32CreateCaret@4',164:'_WU32DestroyCaret@4',165:'_WU32SetCaretPos@4',
           166:'_WU32HideCaret@4',167:'_WU32ShowCaret@4',168:'_WU32SetCaretBlinkTime@4',
           169:'_WU32GetCaretBlinkTime@4',183:'_WU32GetCaretPos@4'}
    table_obj=h['Coff'](B/'obj/wowtbl.obj')
    definitions=[s for s in table_obj.symbols.values() if s[0]=='_aw32WOW' and s[2]>0]
    assert len(definitions)==1
    _,start,section=definitions[0];body,relocs=table_obj.sections[section-1]
    table=next(s for s in symbols if s['name']=='_aw32WOW' and s['owner'].lower()=='wowtbl.obj')
    slots=[]
    for ordinal,name in names.items():
        positions=[p for p,n,k in relocs if n==name and k==6]
        assert len(positions)==1
        position=positions[0];assert position-start==(311+ordinal)*4
        target=next(s for s in mapped if s['name']==name)
        value=struct.unpack('<I',h['data_at'](image,table['va']+position-start,4))[0]
        assert value==(target['va']+struct.unpack_from('<I',body,position)[0])&0xffffffff
        slots.append(dict(user_ordinal=ordinal,table_index=311+ordinal,symbol=name,va=value))
    calls=[]
    for caller_name,target_name in (('_WU32CreateCaret@4','_wow_native_CreateCaret@16'),
        ('_WU32DestroyCaret@4','_wow_native_DestroyCaret@0'),('_WU32SetCaretPos@4','_wow_native_SetCaretPos@8'),
        ('_WU32HideCaret@4','_wow_native_HideCaret@4'),('_WU32ShowCaret@4','_wow_native_ShowCaret@4')):
        caller=next(s for s in mapped if s['name']==caller_name)
        target=next(s for s in symbols if s['name']==target_name and s['function'])
        code,relocs=obj.function(caller_name);positions=[p for p,n,k in relocs if n==target_name and k==20]
        assert len(positions)==1
        pos=positions[0];assert code[pos-1] in (0xe8,0xe9)
        displacement=struct.unpack('<i',h['data_at'](image,caller['va']+pos,4))[0]
        assert caller['va']+pos+4+displacement==target['va']+struct.unpack_from('<i',code,pos)[0]
        calls.append(dict(caller=caller_name,target=target_name,target_owner=target['owner']))
    return dict(result='pass',sources=sources,functions=functions,user_slots=slots,native_calls=calls,
        inputs=[dict(path=str(p),sha256=sha(p)) for p in dict.fromkeys(inputs)],
        scope='Initial caret dispatch and selected adapter calls; historic identical code section is not current-run guest execution')


if __name__=='__main__':
    print(json.dumps(inspect(),indent=2))
