import csv,io,json,subprocess
from pathlib import Path
b=Path('artifacts/mvdm-audit')
def snapshot(name):
 data=subprocess.check_output(['git','show','87eae630b:'+str(b/name).replace('\\','/')])
 return csv.DictReader(io.StringIO(data.decode('utf-8-sig')))
c=list(snapshot('combined-initial-status.csv'));ret=[]
for s,f in [('0','review.csv'),('1','file-audit.csv'),('2','file-audit.csv'),('3','file-review.csv')]:
 d={r['relative_path']:r for r in snapshot('shard-'+s+'/'+f)}
 for x in c:
  if x['shard']!=s or x['status']!='partial' or x['relative_path'] not in d:continue
  r=d[x['relative_path']]; reason=r.get('finding',r.get('decision_basis',r.get('review_basis','')))
  if not ('selected-guest-image-partial' in reason or 'selected-guest-data-partial' in reason):continue
  ret.append(dict(shard=s,path=x['relative_path'],reason=reason,recorded_runtime=r.get('behavior_evidence','not stated in this row'),remaining=r.get('remaining_evidence',r.get('review_limits',r.get('remaining_proof','')))))
print(json.dumps(dict(rows=ret,limits='Only row-reported image/data identity basis; absence of behavioral evidence in a row is not proof no independent witness exists; mirror diffs are not selected.'),ensure_ascii=True,indent=2))
