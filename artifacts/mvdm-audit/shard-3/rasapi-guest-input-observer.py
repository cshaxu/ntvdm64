import csv,hashlib,json
from pathlib import Path
root=Path(__file__).resolve().parents[3]
paths=['wow16/rasapi16/bseerr.h','wow16/rasapi16/ras.h','wow16/rasapi16/rasapi16.rc','wow16/rasapi16/raserror.h']
with (root/'artifacts/mvdm-audit/shard-3/file-review.csv').open(encoding='utf-8-sig',newline='') as f: rows={r['relative_path']:r for r in csv.DictReader(f)}
out=[]
for p in paths:
 r=rows[p]; h=hashlib.sha256((root/'src/mvdm'/p).read_bytes()).hexdigest().upper()
 out.append({'path':p,'sha256':h,'opennt_sha256':r['opennt_sha256'],'opennt45_sha256':r['opennt45_sha256']})
print(json.dumps(out,indent=2))
