import csv,io,json,subprocess
from pathlib import Path
b=Path('artifacts/mvdm-audit')
def snapshot(name):
 data=subprocess.check_output(['git','show','b8aaf0870:'+str(b/name).replace('\\','/')])
 return csv.DictReader(io.StringIO(data.decode('utf-8-sig')))
combined=list(snapshot('combined-initial-status.csv'));out=[]
exts=('.c','.h','.asm','.inc','.rc','.dlg','.equ','.str','.ext','.skl','.rcv','.def','.bmp','.res','.ico','.cur','.msg','.bin','.ver')
for s,f in [('0','review.csv'),('1','file-audit.csv'),('2','file-audit.csv'),('3','file-review.csv')]:
 rows={r['relative_path']:r for r in snapshot('shard-'+s+'/'+f)}
 for z in combined:
  p=z['relative_path'];r=rows.get(p)
  if z['shard']!=s or z['status']!='excluded' or not r or not p.startswith(('dos/v86/','wow16/')) or not p.lower().endswith(exts): continue
  if '/tests/' in p or p.startswith(('dos/v86/tools/','wow16/test/hello/','wow16/wowdeb/')):continue
  text=' '.join(r.get(k,'') for k in ('finding','classification_basis','decision_basis','review_basis','remaining_evidence','next_required_evidence','review_limits','remaining_proof'))
  low=text.lower();bounded=any(t in low for t in ('historical guest/tool build-input file role excluded','historical guest-build source role is excluded from host implementation inputs','historical-guest-build-input-only','historical-guest-build-source-file-only','historical guest include/build-input role excluded'))
  if not bounded:continue
  out.append(dict(shard=s,path=p,raw_evidence=text,prior_review=z['review_file']))
print(json.dumps(dict(rows=out,counts={s:sum(x['shard']==s for x in out) for s in '0123'},snapshot='combined-initial-status.csv as read at execution',limits='Source/resource candidates only; explicit scoped evidence predicate; tests/tools deliberately excluded; no other guest data categories adjudicated.'),ensure_ascii=True,indent=2))
