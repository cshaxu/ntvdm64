import hashlib,json
from pathlib import Path
p=Path('artifacts/mvdm-audit/bios-image-observations.json');d=json.loads(p.read_text())
indices=list(range(0x34,0x40))+list(range(0xA0,0xA8))
slots=[x for x in d['bios_slots'] if x['index'] in indices]
assert len(slots)==len(indices)
r={'source_path':str(p).replace('\\','/'),'source_sha256':hashlib.sha256(p.read_bytes()).hexdigest().upper(),
   'original_inputs':d['inputs'],'scope':d['scope'],'slots':slots,
   'all_illegal_bop':all(x['symbol']=='_illegal_bop' for x in slots)}
Path('artifacts/mvdm-audit/host-inc/legacy-network-bop-evidence.json').write_text(json.dumps(r,indent=2)+'\n',encoding='utf-8')
print('slots:',len(slots),'all illegal:',r['all_illegal_bop'])
