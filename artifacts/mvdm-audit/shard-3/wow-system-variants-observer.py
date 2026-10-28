import csv,hashlib,json
from pathlib import Path
root=Path(__file__).resolve().parents[3]
paths=['wow16/system/envect.asm','wow16/system/hpsystem','wow16/system/hpsystem.def','wow16/system/romsys','wow16/system/system.drv']
with (root/'artifacts/mvdm-audit/shard-3/file-review.csv').open(encoding='utf-8-sig',newline='') as f: rows={r['relative_path']:r for r in csv.DictReader(f)}
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper()
out=[]
for p in paths:
 r=rows[p];h=sha(root/'src/mvdm'/p)
 out.append({'path':p,'sha256':h,'opennt_sha256':r['opennt_sha256'],'opennt45_sha256':r['opennt45_sha256'],'origin':r['upstream_origin']})
print(json.dumps({'files':out,'installed_system_drv_sha256':sha(Path('O:/winnt/system32/system.drv'))},indent=2))
