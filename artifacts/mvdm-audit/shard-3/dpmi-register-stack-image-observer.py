"""Check original DPMI register/stack code and callers in released NTVDM."""
import csv
import hashlib
import json
import runpy
import struct
import subprocess
from pathlib import Path

A=Path('artifacts/mvdm-audit')
B=Path('build/.ninja/msvc-x86-release/worker')


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    h=runpy.run_path(str(A/'host-tail-image-observer.py'))
    frozen={r['relative_path']:r for r in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    graph=(B/'build.ninja').read_text().replace('$:',':')
    archive=next(line for line in graph.splitlines() if line.startswith('build original-mvdm-dpmi32.lib:'))
    deps=subprocess.check_output(['ninja','-C',str(B),'-t','deps'],text=True)
    image_path=Path('assets/release/ntvdm.exe')
    image=h['Image'](image_path)
    symbols=h['symbols']((B/'ntvdm.exe.map').read_text())
    inputs=[image_path,B/'build.ninja',B/'ntvdm.exe.map']
    results=[]
    for name,expected in (('register',8),('stack',4)):
        relative='dpmi32/'+name+'.c'
        current=Path('src/mvdm')/relative
        first=Path('O:/repos.external/OpenNT/base/mvdm')/relative
        second=Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')/relative
        record=frozen[relative]
        assert sha(current)==sha(first)==sha(second)==record['current_sha256']==record['opennt_sha256']==record['opennt45_sha256']
        assert record['dos_catalog_rows']=='1' and record['wow_catalog_rows']=='2'
        target='obj/dpmi/'+name+'.obj'
        edge=next(line for line in graph.splitlines() if line.startswith('build '+target+':'))
        assert target in archive.split() and current.resolve().as_posix().lower() in edge.lower()
        dep=next(line for line in deps.splitlines() if line.startswith(target+':'))
        assert '(VALID)' in dep and '(STALE)' not in dep
        obj=h['Coff'](B/target)
        mapped=[s for s in symbols if s['function'] and s['owner'].lower()==('original-mvdm-dpmi32:'+name+'.obj').lower()]
        assert len(mapped)==expected
        results.append(dict(relative_path=relative,sha256=sha(current),wow_source_ids=record['wow_source_ids'],
            original_path=str(first),compile_edge=edge,dependency_header=dep,
            functions=[h['bind'](image,obj,s) for s in mapped]))
        inputs.extend([current,first,second,B/target])
    calls=[]
    requests=(('dpmi32','_DpmiInitApp','_DpmiInitRegisterSize'),
              ('buffer','_DpmiSegmentToSelector','_DpmiSwitchToDosxStack'),
              ('buffer','_DpmiSegmentToSelector','_DpmiSwitchFromDosxStack'),
              ('int21map','_NoTranslation','_DpmiSwitchToDosxStack'),
              ('int21map','_NoTranslation','_DpmiSwitchFromDosxStack'),
              ('int21map','_NoTranslation','_DpmiSimulateIretCF'))
    for objname,caller_name,target_name in requests:
        owner='original-mvdm-dpmi32:'+objname+'.obj'
        caller=next(s for s in symbols if s['name']==caller_name and s['owner']==owner)
        target=next(s for s in symbols if s['name']==target_name and s['owner'].startswith('original-mvdm-dpmi32:'))
        obj=h['Coff'](B/('obj/dpmi/'+objname+'.obj'))
        body,relocs=obj.function(caller_name)
        positions=[p for p,n,k in relocs if n==target_name and k==20]
        assert len(positions)==1
        offset=positions[0]
        assert body[offset-1] in (0xe8,0xe9)
        displacement=struct.unpack('<i',h['data_at'](image,caller['va']+offset,4))[0]
        assert caller['va']+offset+4+displacement==target['va']+struct.unpack_from('<i',body,offset)[0]
        calls.append(dict(caller=caller_name,target=target_name,caller_owner=owner,va=caller['va']+offset-1))
        inputs.append(B/('obj/dpmi/'+objname+'.obj'))
    return dict(result='pass',sources=results,calls=calls,
        inputs=[dict(path=str(p),sha256=sha(p)) for p in dict.fromkeys(inputs)],
        scope='Selected code identity and six resolved callers; no complete stack, exception or lifecycle behavior')


if __name__=='__main__':
    print(json.dumps(inspect(),indent=2))
