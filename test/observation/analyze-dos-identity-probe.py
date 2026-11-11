"""Assert the observed identity failure; this is not a product-pass verdict."""
from pathlib import Path
import json, re, struct, hashlib
root=Path(__file__).resolve().parents[2]
base=root/'build/side-dos-identity'
summary=[]
for case in ['dos1-b','dos0-a']:
    folder=base/case
    witnesses={r:struct.unpack('<BHH',(folder/f'{r}.RUN').read_bytes()) for r in 'AB'}
    assert witnesses['A'][0]==ord('A') and witnesses['B'][0]==ord('B')
    assert witnesses['A'][2]!=witnesses['B'][2]
    assert witnesses['A'][1]==witnesses['B'][1]
    for r in 'AB':
        assert (folder/f'{r}.END').read_bytes()==(folder/f'{r}.RUN').read_bytes()
    psp=witnesses['A'][1]
    rows=[]
    for line in (folder/'after-b-events.log').read_text().splitlines():
        event=line.split()[0]
        fields=dict(re.findall(r'(\w+)=(\w+)',line))
        for k in fields:
            fields[k]=int(fields[k],16 if k in ['psp','parent','env','cr0','cr3','linear','physical'] else 10)
        if fields['psp']==psp:
            rows.append(dict(event=event,**fields))
    assert len({r['pid'] for r in rows})==1
    creates=[r for r in rows if r['event']=='create-after']
    exits=[r for r in rows if r['event']=='exit-before']
    assert len(creates)==len(exits)==2
    a,b=creates; ea,eb=exits
    assert a['physical']!=b['physical'] and a['cr3']==b['cr3']
    assert ea['physical']==a['physical'] and eb['physical']==b['physical']
    assert ea['occurrence']==b['occurrence'] and ea['occurrence']!=a['occurrence']
    assert eb['occurrence']==0
    item=dict(case=case,worker=a['pid'],guest_vm_a=witnesses['A'][2],guest_vm_b=witnesses['B'][2],
        psp=f'{psp:04X}',cr3=f'{a["cr3"]:08X}',physical_a=f'{a["physical"]:08X}',physical_b=f'{b["physical"]:08X}',
        occurrence_a=a['occurrence'],occurrence_b=b['occurrence'],
        exit_a_selected=ea['occurrence'],exit_b_selected=eb['occurrence'],
        observation_collision_proved=True)
    if case.startswith('dos0'):
        before=json.loads((folder/'both-run16.json').read_text(encoding='utf-8-sig'))
        after=json.loads((folder/'after-a-run16.json').read_text(encoding='utf-8-sig'))
        if isinstance(after,dict): after=[after]
        before=[p for p in before if p['ParentProcessId']==a['pid']]
        after=[p for p in after if p['ParentProcessId']==a['pid']]
        assert len(before)==2 and len(after)==1
        assert after[0]['CommandLine'].endswith('A.com')
        assert any(p['CommandLine'].endswith('B.com') for p in before)
        item['wrong_run16_retirement_proved']=True
    item['evidence_sha256']={p.name:hashlib.sha256(p.read_bytes()).hexdigest()
        for p in folder.iterdir() if p.is_file() and p.suffix in ['.log','.RUN','.END','.json']}
    summary.append(item)
(base/'findings.json').write_text(json.dumps(summary,indent=2)+'\n')
print(json.dumps(summary,indent=2))
