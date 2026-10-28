import csv,io,json,subprocess
from pathlib import Path
b=Path('artifacts/mvdm-audit')
def snapshot(name):
 data=subprocess.check_output(['git','show','b8aaf0870:'+str(b/name).replace('\\','/')])
 return csv.DictReader(io.StringIO(data.decode('utf-8-sig')))
c=list(snapshot('combined-initial-status.csv'));out=[]
ignore={'.obj','.map','.sym','.lib','.lnk','.lrf','.lst','.pdb','.rst','.tag','.tmp'}
for s,f in [('0','review.csv'),('1','file-audit.csv'),('2','file-audit.csv'),('3','file-review.csv')]:
 rows={r['relative_path']:r for r in snapshot('shard-'+s+'/'+f)}
 for z in c:
  p=z['relative_path'];r=rows.get(p)
  if z['shard']!=s or z['status']!='excluded' or not r or not p.startswith(('dos/v86/','wow16/')) or Path(p).suffix.lower() in ignore:continue
  if '/tests/' in p or p.startswith(('dos/v86/tools/','wow16/test/hello/','wow16/wowdeb/')):continue
  text=' '.join(r.get(k,'') for k in ('finding','classification_basis','decision_basis','review_basis','remaining_evidence','next_required_evidence','review_limits','remaining_proof'))
  out.append(dict(shard=s,path=p,basis=text,role=r.get('role_or_prior_finding',''),evidence=r.get('evidence',r.get('reviewed_test_evidence',r.get('specific_evidence','')))))
print(json.dumps(dict(rows=out,limits='Current guest excluded inventory except selected historical objects, listing, archives, linker inputs, sample/test and standalone tool paths; does not infer exclusion is wrong.'),ensure_ascii=True,indent=2))
