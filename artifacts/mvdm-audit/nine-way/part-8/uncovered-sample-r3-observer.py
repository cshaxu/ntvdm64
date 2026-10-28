import csv,json,pathlib,hashlib,collections
r=pathlib.Path('artifacts/mvdm-audit');p=r/'nine-way/part-8';exclude=set()
for f in ['existing-classifications-sample100.csv','existing-classifications-sample101-300.csv','existing-classifications-uncovered-r2.csv']:
 with (p/f).open(encoding='utf-8-sig',newline='') as h:exclude.update(x['path'].lower() for x in csv.DictReader(h))
for x in json.loads((r/'nine-way/part-1/other-guest-excluded-scope-evidence.json').read_text(encoding='utf-8-sig'))['rows']:exclude.add(x['path'].lower())
files=[('0','shard-0/review.csv','classification'),('1','shard-1/file-audit.csv','audit_status'),('2','shard-2/file-audit.csv','decision'),('3','shard-3/file-review.csv','review_class')];out=[];hashes=[]
for shard,f,k in files:
 q=r/f;hashes.append({'path':f,'sha256':hashlib.sha256(q.read_bytes()).hexdigest().upper()})
 with q.open(encoding='utf-8-sig',newline='') as h:
  for x in csv.DictReader(h):
   name=x['relative_path'];v=x[k]
   if v not in ('partial','部分复通','excluded','整体排除') or name.lower() in exclude:continue
   out.append({'path':name,'shard':shard,'classification':v,'evidence':x.get('decision_basis') or x.get('review_basis') or x.get('evidence') or x.get('role_or_prior_finding') or '', 'limit':x.get('review_limits') or x.get('remaining_proof') or x.get('remaining_evidence') or x.get('next_required_evidence') or ''})
out.sort(key=lambda x:x['path'].lower());out=out[:100];dest=p/'existing-classifications-uncovered-r3.csv'
if dest.exists():raise SystemExit('Frozen sample exists, do not overwrite')
with dest.open('w',encoding='utf-8',newline='') as f:
 w=csv.DictWriter(f,fieldnames=['path','shard','classification','evidence','limit']);w.writeheader();w.writerows(out)
meta={'source_hashes':hashes,'sample_sha256':hashlib.sha256(dest.read_bytes()).hexdigest().upper(),'exclude_lists':['sample100-all','sample101-300-all','part1-other-guest-564'],'rows':len(out)}
(p/'existing-classifications-uncovered-r3-baseline.json').write_text(json.dumps(meta,indent=2),encoding='utf-8')
print(len(out),collections.Counter('/'.join(x['path'].split('/')[:4]) for x in out))
