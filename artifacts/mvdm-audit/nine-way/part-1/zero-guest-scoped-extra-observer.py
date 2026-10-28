import csv,io,json,subprocess
from pathlib import Path
b=Path('artifacts/mvdm-audit')
def snapshot(name):
 data=subprocess.check_output(['git','show','952d68c5a:'+str(b/name).replace('\\','/')])
 return csv.DictReader(io.StringIO(data.decode('utf-8-sig')))
c={r['relative_path'] for r in snapshot('combined-initial-status.csv') if r['shard']=='0' and r['status']=='excluded'};ret=[]
for r in snapshot('shard-0/review.csv'):
 p=r['relative_path'];rem=r['remaining_evidence'].lower()
 if p not in c or r['classification']!='excluded' or not p.lower().endswith(('.c','.h','.asm','.inc')) or Path(p).name.lower()=='make.inc':continue
 if 'only historical guest/tool build-input file role excluded' not in rem and 'only this physical historical guest include/build-input role excluded' not in rem:continue
 ret.append(dict(path=p,finding=r['finding'],evidence=r['evidence'],remaining=r['remaining_evidence']))
print(json.dumps(dict(rows=ret,limits='Explicit scoped guest declaration/source only; excludes make.inc build recipe and does not adjudicate all other items.'),ensure_ascii=True,indent=2))
