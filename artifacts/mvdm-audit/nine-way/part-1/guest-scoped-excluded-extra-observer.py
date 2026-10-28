import csv,io,json,subprocess
from pathlib import Path
b=Path('artifacts/mvdm-audit')
def snapshot(name):
 data=subprocess.check_output(['git','show','a87018f30:'+str(b/name).replace('\\','/')])
 return csv.DictReader(io.StringIO(data.decode('utf-8-sig')))
c=list(snapshot('combined-initial-status.csv'));r={x['relative_path']:x for x in snapshot('shard-1/file-audit.csv')};out=[]
for x in c:
 p=x['relative_path']
 if x['shard']!='1' or x['status']!='excluded' or p not in r or not p.lower().endswith(('.c','.h','.asm','.inc')):continue
 row=r[p];text=(row['classification_basis']+' '+row['role_or_prior_finding']+' '+row['next_required_evidence']).lower()
 if any(t in text for t in ('historical guest/tool build-input file role excluded','historical guest include/build-input role excluded','historical guest-build source role is excluded from host implementation inputs')):
  out.append(dict(path=p,basis=row['classification_basis'],prior=row['role_or_prior_finding'],evidence=row['evidence'],remaining=row['next_required_evidence']))
print(json.dumps(dict(rows=out,limits='Scoped guest source/declaration evidence only; not all remaining exclusions audited, no capability restored.'),ensure_ascii=True,indent=2))
