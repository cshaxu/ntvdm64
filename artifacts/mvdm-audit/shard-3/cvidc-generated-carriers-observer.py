import csv,hashlib,json
from pathlib import Path
root=Path(__file__).resolve().parents[3]
names=['acc_efi.h','axphst_c.h','bpi_c.h','c2cpusad.h','ccpudefs.inc','parser.h','trace_c.h']
with (root/'artifacts/mvdm-audit/shard-3/file-review.csv').open(encoding='utf-8-sig',newline='') as f: rows={r['relative_path']:r for r in csv.DictReader(f)}
def digest(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper()
out=[]
for n in names:
 key='softpc.new/base/cvidc/'+n;p=root/'src/mvdm'/key; r=rows[key]; h=digest(p)
 dup=[]
 for q in (root/'src/mvdm/softpc.new').rglob(n):
  if q!=p and digest(q)==h:dup.append(str(q.relative_to(root)).replace('\\','/'))
 out.append({'path':key,'sha256':h,'opennt_sha256':r['opennt_sha256'],'opennt45_sha256':r['opennt45_sha256'],'identical_named_copies':dup})
print(json.dumps(out,indent=2))
