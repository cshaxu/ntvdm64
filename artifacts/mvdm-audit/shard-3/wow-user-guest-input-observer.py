import csv,hashlib,json
from pathlib import Path
root=Path(__file__).resolve().parents[3]
paths=['wow16/user/editml.c','wow16/user/edslrare.c','wow16/user/k/daytona/USER.EXE','wow16/user/msglayer.inc','wow16/user/msutil.c','wow16/user/wowasm.sed','wow16/user/wowasmc.cmd']
with (root/'artifacts/mvdm-audit/shard-3/file-review.csv').open(encoding='utf-8-sig',newline='') as f: rows={r['relative_path']:r for r in csv.DictReader(f)}
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper()
out=[]
for p in paths:
 r=rows[p];out.append({'path':p,'sha256':sha(root/'src/mvdm'/p),'opennt_sha256':r['opennt_sha256'],'opennt45_sha256':r['opennt45_sha256'],'origin':r['upstream_origin']})
print(json.dumps({'files':out,'installed_user_exe_sha256':sha(Path('O:/winnt/system32/USER.EXE'))},indent=2))
