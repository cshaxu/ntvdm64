import csv,io,json,subprocess
from pathlib import Path
b=Path('artifacts/mvdm-audit')
def snapshot(name):
 data=subprocess.check_output(['git','show','952d68c5a:'+str(b/name).replace('\\','/')])
 return csv.DictReader(io.StringIO(data.decode('utf-8-sig')))
combined=list(snapshot('combined-initial-status.csv'));out=[]
for s,f in [('2','file-audit.csv'),('3','file-review.csv')]:
 rows={r['relative_path']:r for r in snapshot('shard-'+s+'/'+f)}
 for z in combined:
  p=z['relative_path'];r=rows.get(p)
  if z['shard']!=s or z['status']!='excluded' or not r or not p.lower().endswith(('.c','.h','.asm','.inc')):continue
  parts=[r.get(k,'') for k in ('classification_basis','decision_basis','review_basis','remaining_proof','review_limits','role_boundary')];text=' '.join(parts).lower()
  tokens=('historical guest/tool build-input file role excluded','historical guest include/build-input role excluded','historical guest-build source role is excluded from host implementation inputs')
  if any(t in text for t in tokens):out.append(dict(shard=s,path=p,basis=parts))
print(json.dumps(dict(rows=out,limits='Explicit scoped guest-only reasons; current aggregate snapshot, no capability or guest runtime claim.'),ensure_ascii=True,indent=2))
