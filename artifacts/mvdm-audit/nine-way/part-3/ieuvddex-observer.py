import csv,hashlib,json
from pathlib import Path
repo=Path(__file__).resolve().parents[4]
roots={'current':repo/'src/mvdm','opennt':Path('O:/repos.external/OpenNT/base/mvdm'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')}
with (repo/'artifacts/mvdm-audit/nine-way-ownership.csv').open(encoding='utf-8-sig',newline='') as f: rows=[r for r in csv.DictReader(f) if r['kind']=='mvdm' and r['bucket']=='3' and r['path'].startswith('ieuvddex/')]
def hashfile(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else None
out=[]
for r in rows:
 out.append({'key':r['key'],'path':r['path'],'sha256':{name:hashfile(root/r['path']) for name,root in roots.items()}})
print(json.dumps(out,indent=2))
