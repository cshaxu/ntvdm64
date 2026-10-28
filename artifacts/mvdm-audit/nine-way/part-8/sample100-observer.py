import csv,io,pathlib,subprocess
root=pathlib.Path('artifacts/mvdm-audit');p=root/'nine-way/part-8'
files=[('0','shard-0/review.csv','classification'),('1','shard-1/file-audit.csv','audit_status'),('2','shard-2/file-audit.csv','decision'),('3','shard-3/file-review.csv','review_class')]
rows=[]
for shard,filename,field in files:
 data=subprocess.check_output(['git','show','7a0a22f32:'+str(root/filename).replace('\\','/')])
 with io.StringIO(data.decode('utf-8-sig'),newline='') as f:
  for r in csv.DictReader(f):
   c=r[field]
   if c not in ('partial','excluded','部分复通','整体排除'):continue
   rows.append({'path':r['relative_path'],'shard':shard,'classification':c,'evidence':r.get('decision_basis') or r.get('review_basis') or r.get('evidence') or r.get('role_or_prior_finding') or '', 'limit':r.get('review_limits') or r.get('remaining_proof') or r.get('remaining_evidence') or r.get('next_required_evidence') or ''})
rows.sort(key=lambda x:x['path'].lower()); rows=rows[:100]
with (p/'existing-classifications-sample100.csv').open('w',encoding='utf-8',newline='') as f:
 w=csv.DictWriter(f,fieldnames=['path','shard','classification','evidence','limit']);w.writeheader();w.writerows(rows)
from collections import Counter
print('rows',len(rows),'class',Counter(x['classification'] for x in rows));print('prefix',Counter('/'.join(x['path'].split('/')[:2]) for x in rows))
