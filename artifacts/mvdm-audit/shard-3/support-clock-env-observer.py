"""Reconcile original clock and environment helper against released NTVDM."""
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
NAMES=('time_day','get_env')


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    h=runpy.run_path(str(A/'host-tail-image-observer.py'))
    frozen={r['relative_path']:r for r in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    graph=(B/'build.ninja').read_text().replace('$:',':')
    archive=next(s for s in graph.splitlines() if s.startswith('build original-softpc-support.lib:'))
    flags=next(s for s in graph.splitlines() if s.startswith('cflags ='))
    assert '/DNTVDM' in flags and '/DPROD' in flags and not re.search(r'/DPIG(?:\s|$)',flags)
    deptext=subprocess.check_output(['ninja','-C',str(B),'-t','deps'],text=True)
    image=h['Image'](Path('assets/release/ntvdm.exe'))
    symbols=h['symbols']((B/'ntvdm.exe.map').read_text())
    inputs=[Path('assets/release/ntvdm.exe'),B/'ntvdm.exe.map',B/'build.ninja']
    rows=[]
    for name in NAMES:
        relative='softpc.new/base/support/'+name+'.c'
        source=Path('src/mvdm')/relative
        one=Path('O:/repos.external/OpenNT/base/mvdm')/relative
        two=Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')/relative
        record=frozen[relative]
        assert sha(source)==sha(one)==sha(two)==record['current_sha256']==record['opennt_sha256']==record['opennt45_sha256']
        assert record['dos_catalog_rows']=='1' and record['wow_catalog_rows']=='2'
        target='obj/support/'+name+'.obj'
        edge=next(s for s in graph.splitlines() if s.startswith('build '+target+':'))
        assert target in archive.split() and source.resolve().as_posix().lower() in edge.lower()
        dep=next(s for s in deptext.splitlines() if s.startswith(target+':'))
        assert '(VALID)' in dep and '(STALE)' not in dep
        obj=h['Coff'](B/target)
        mapped=[s for s in symbols if s['function'] and s['owner'].lower()==('original-softpc-support:'+name+'.obj').lower()]
        funcs=[h['bind'](image,obj,s) for s in mapped]
        assert len(funcs)==(3 if name=='time_day' else 0)
        rows.append(dict(relative_path=relative,sha256=sha(source),wow_source_ids=record['wow_source_ids'],
                         original_path=str(one),compile_edge=edge,dependency_header=dep,functions=funcs))
        inputs.extend([source,one,two,B/target])
    # Pin the original BIOS BOP 1Ah initial vector to the linked clock owner.
    bios=h['Coff'](B/'obj/bios/bios.obj')
    definition=[s for s in bios.symbols.values() if s[0]=='_BIOS' and s[2]>0]
    assert len(definition)==1
    _,start,section=definition[0]
    body,relocs=bios.sections[section-1]
    slot=start+0x1a*4
    relocation=[r for r in relocs if r[0]==slot]
    assert len(relocation)==1 and relocation[0][1:] == ('_time_of_day',6)
    table=next(s for s in symbols if s['name']=='_BIOS' and s['owner']=='original-softpc-bios:bios.obj')
    clock=next(s for s in symbols if s['name']=='_time_of_day' and s['owner']=='original-softpc-support:time_day.obj')
    actual=struct.unpack('<I',h['data_at'](image,table['va']+0x1a*4,4))[0]
    assert actual==(clock['va']+struct.unpack_from('<I',body,slot)[0])&0xffffffff
    inputs.append(B/'obj/bios/bios.obj')
    search=subprocess.run(['rg','-n',r'\b(IBOOLgetenv|ISM32getenv|STRINGgetenv)\b','src','-g','*.[ch]'],
                          text=True,capture_output=True,check=True)
    matches=search.stdout.splitlines()
    assert len(matches)==7 and sum('ccpupig.c:' in line for line in matches)==1
    pig=Path('src/mvdm/softpc.new/base/ccpu386/ccpupig.c')
    pigtext=pig.read_text(encoding='latin1')
    assert pigtext.index('#ifdef\tPIG') < pigtext.index('ISM32getenv(')
    inputs.extend([pig,Path('src/mvdm/softpc.new/base/bios/bios.c'),
        Path('src/mvdm/softpc.new/base/bios/reset.c'),Path('src/mvdm/softpc.new/host/src/config.c'),
        Path('src/mvdm/softpc.new/host/inc/host_def.h'),Path('src/ntvdm-exe/win32/mvdm_crt_redirect.h')])
    return dict(result='pass',sources=rows,bios_1a_target=clock,flags=flags,
        helper_search=matches,inputs=[dict(path=str(x),sha256=sha(x)) for x in inputs],
        scope='Clock BOP initial image binding; source profile and helper linkage, not timing behavior')


if __name__=='__main__':
    print(json.dumps(inspect(),indent=2))
