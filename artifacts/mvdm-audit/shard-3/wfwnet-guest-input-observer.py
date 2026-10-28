import csv,hashlib,json
from pathlib import Path
root=Path(__file__).resolve().parents[3]
paths=['wow16/wfwnet/WFWNET.DRV','wow16/wfwnet/ints.asm','wow16/wfwnet/wfwnet.def']
with (root/'artifacts/mvdm-audit/shard-3/file-review.csv').open(encoding='utf-8-sig',newline='') as f: rows={r['relative_path']:r for r in csv.DictReader(f)}
def sha(p): return hashlib.sha256(p.read_bytes()).hexdigest().upper()
out=[]
for p in paths:
 r=rows[p];out.append({'path':p,'sha256':sha(root/'src/mvdm'/p),'opennt_sha256':r['opennt_sha256'],'opennt45_sha256':r['opennt45_sha256']})
print(json.dumps({'files':out,'installed_sha256':sha(Path('O:/winnt/system32/WFWNET.DRV')),'standard_vdd_present':Path('O:/winnt/system32/VWIPXSPX.DLL').exists()},indent=2))
