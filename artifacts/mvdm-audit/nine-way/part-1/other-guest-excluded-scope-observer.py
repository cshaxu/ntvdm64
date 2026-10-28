import csv,io,json,subprocess
from pathlib import Path
base=Path('artifacts/mvdm-audit')
def snapshot(name):
 data=subprocess.check_output(['git','show','fb85f484c:'+str(base/name).replace('\\','/')])
 return csv.DictReader(io.StringIO(data.decode('utf-8-sig')))
combined=list(snapshot('combined-initial-status.csv'));out=[]
prefix='Historical immutable guest build source/resource only; excluded as current host implementation input, not as guest functionality.'
for shard,file in [('0','review.csv'),('2','file-audit.csv'),('3','file-review.csv')]:
 rows={r['relative_path']:r for r in snapshot('shard-'+shard+'/'+file)}
 for old in combined:
  if old['shard']!=shard or old['status']!='excluded' or old['relative_path'] not in rows:continue
  r=rows[old['relative_path']]
  if shard=='0':
   if r['classification']!='excluded' or r['finding'] not in ('Original 16-bit guest build/input role; no host feature exclusion or runtime completion claim.','Original guest shared declaration role, no guest API/capability exclusion or native ABI completion.'):continue
   basis=r['finding'];proof=r['remaining_evidence'];evidence=r['evidence']
  else:
   if not r['classification_basis'].startswith(prefix):continue
   if (r['decision'] if shard=='2' else r['review_class'])!='excluded':continue
   basis=r['classification_basis'];proof=r['review_limits'] if shard=='2' else r['remaining_proof'];evidence=r['source_owner_chain'] if shard=='3' else r['decision_basis']
  out.append(dict(shard=shard,path=old['relative_path'],initial_status='excluded',current_scoped_decision='excluded',recommended='pending-overall-preserve-scoped-role',basis=basis,evidence=evidence,remaining=proof))
print(json.dumps(dict(rows=out,counts={s:sum(x['shard']==s for x in out) for s in ('0','2','3')},limits='Exact positive phrases and current excluded status only; other excluded rows not adjudicated, shared ledger untouched.'),ensure_ascii=True,indent=2))
