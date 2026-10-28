import csv,re,pathlib
root=pathlib.Path('artifacts/mvdm-audit')
def rd(p):
 with p.open(encoding='utf-8-sig',newline='') as f:return list(csv.DictReader(f))
initial={r['relative_path']:r for r in rd(root/'combined-initial-status.csv')}
own={r['path']:r for r in rd(root/'nine-way-ownership.csv') if r['kind']=='mvdm' and r['bucket'] in ('2','5','8')}
paths={'0':('shard-0/review.csv','classification'),'1':('shard-1/file-audit.csv','audit_status'),'2':('shard-2/file-audit.csv','decision'),'3':('shard-3/file-review.csv','review_class')}
current={}
for shard,(path,field) in paths.items():
 for row in rd(root/path):current[(shard,row['relative_path'])]=row[field]
rows=[]
for bucket in ('2','5','8'):
 for p in sorted((root/'nine-way'/('part-'+bucket)).glob('*reconciliation.md')):
  for line in p.read_text(encoding='utf-8-sig').splitlines():
   if not line.startswith('| `'):continue
   cols=[x.strip() for x in line.strip('|').split('|')]
   for key in re.findall(r'`([^`]+)`',cols[0]):
    if key not in own or own[key]['bucket']!=bucket or key not in initial:continue
    shard=initial[key]['shard'];live=current.get((shard,key),'MISSING')
    if live not in ('pending','待核验'):continue
    rows.append({'path':key,'bucket':bucket,'source_shard':shard,'current_status':live,'disposition':cols[1] if len(cols)>1 else '', 'report':str(p).replace('\\','/')})
out=root/'nine-way'/'part-2'/'semantic-pending-recommendations.csv'
with out.open('w',encoding='utf-8',newline='') as f:
 w=csv.DictWriter(f,fieldnames=['path','bucket','source_shard','current_status','disposition','report']);w.writeheader();w.writerows(rows)
print('current pending rows',len(rows),'unique',len({r['path'] for r in rows}))
