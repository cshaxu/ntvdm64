import csv,hashlib,json
from pathlib import Path
root=Path(__file__).resolve().parents[3]
paths=['wow16/mciole/MCIOLE16.RES','wow16/mciole/ole.h','wow16/mmsystem/int31.inc','wow16/mmsystem/mixmgri.h','wow16/mmsystem/mmiocf.c','wow16/mmsystem/mmiocf.h','wow16/mmsystem/mmsex.dlg','wow16/mmsystem/mmtask/MMTASK.RES']
with (root/'artifacts/mvdm-audit/shard-3/file-review.csv').open(encoding='utf-8-sig',newline='') as f: rows={r['relative_path']:r for r in csv.DictReader(f)}
out=[]
for p in paths:
 b=(root/'src/mvdm'/p).read_bytes(); r=rows[p]
 out.append({'path':p,'sha256':hashlib.sha256(b).hexdigest().upper(),'opennt_sha256':r['opennt_sha256'],'opennt45_sha256':r['opennt45_sha256'],'origin':r['upstream_origin']})
print(json.dumps(out,indent=2))
