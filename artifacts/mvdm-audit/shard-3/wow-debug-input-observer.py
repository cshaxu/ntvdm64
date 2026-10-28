import csv,hashlib,json
from pathlib import Path
root=Path(__file__).resolve().parents[3]
paths=['wow16/inc/debug.h','wow16/inc/debugsys.inc','wow16/inc/logerror.h']
with (root/'artifacts/mvdm-audit/shard-3/file-review.csv').open(encoding='utf-8-sig',newline='') as f: rows={r['relative_path']:r for r in csv.DictReader(f)}
out=[]
for p in paths:
 r=rows[p];out.append({'path':p,'sha256':hashlib.sha256((root/'src/mvdm'/p).read_bytes()).hexdigest().upper(),'opennt_sha256':r['opennt_sha256'],'opennt45_sha256':r['opennt45_sha256']})
print(json.dumps(out,indent=2))
