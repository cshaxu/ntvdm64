import csv,hashlib,json
from pathlib import Path
repo=Path(__file__).resolve().parents[4]; roots={'current':repo/'src/mvdm','opennt':Path('O:/repos.external/OpenNT/base/mvdm'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')}
names={'inc/WOWMMCB.INC','inc/WOWMMED.INC','inc/wowmmed.h','inc/WOWSND.INC','inc/wowsnd.h'}
with (repo/'artifacts/mvdm-audit/nine-way-ownership.csv').open(encoding='utf-8-sig',newline='') as f: rows=[r for r in csv.DictReader(f) if r['kind']=='mvdm' and r['bucket']=='3' and r['path'] in names]
def h(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else None
print(json.dumps([{'key':r['key'],'path':r['path'],'sha256':{n:h(p/r['path']) for n,p in roots.items()}} for r in rows],indent=2))
