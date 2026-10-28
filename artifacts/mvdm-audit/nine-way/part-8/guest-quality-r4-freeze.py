import csv,json,pathlib,hashlib,collections
r=pathlib.Path('artifacts/mvdm-audit');p=r/'nine-way/part-8';excluded=set();names=['existing-classifications-sample100.csv','existing-classifications-sample101-300.csv','existing-classifications-uncovered-r2.csv','existing-classifications-uncovered-r3.csv']
for n in names:
 with (p/n).open(encoding='utf-8-sig',newline='') as f: excluded.update(x['path'].lower() for x in csv.DictReader(f))
q=r/'nine-way/part-1/other-guest-excluded-scope-evidence.json'; excluded.update(x['path'].lower() for x in json.loads(q.read_text(encoding='utf-8-sig'))['rows'])
ledger=r/'combined-initial-status.csv'; rows=list(csv.DictReader(ledger.open(encoding='utf-8-sig')))
frozen={x['relative_path'].lower():x for x in csv.DictReader((r/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
selected=[]
for x in rows:
 n=x['relative_path'];k=n.lower();t=pathlib.PurePosixPath(k).suffix
 if k in excluded or not k.startswith('dos/v86/'): continue
 if x['status']=='partial' or (x['status']=='excluded' and t in ('.c','.h','.asm','.inc','.skl','.cl1','.cl2','.cl3','.cl4','.cl5')):
  old=frozen.get(k,{}); selected.append(dict(**x,old_sha256=old.get('opennt_sha256',''),alt_sha256=old.get('opennt45_sha256',''),current_sha256=old.get('current_sha256',''),identity=old.get('current_identity','')))
selected.sort(key=lambda x:(x['status']!='partial',x['relative_path'].lower()));selected=selected[:100]
out=p/'guest-quality-r4-frozen.csv'
if out.exists(): raise SystemExit('existing frozen sample')
with out.open('w',encoding='utf-8',newline='') as f:
 w=csv.DictWriter(f,fieldnames=list(selected[0]));w.writeheader();w.writerows(selected)
inputs=[ledger,r/'frozen-file-reconciliation.csv',q]+[p/n for n in names]+[pathlib.Path(__file__)]
(p/'guest-quality-r4-baseline.json').write_text(json.dumps({'files':[{'path':str(z),'sha256':hashlib.sha256(z.read_bytes()).hexdigest()} for z in inputs], 'sample_sha256':hashlib.sha256(out.read_bytes()).hexdigest(),'count':len(selected)},indent=2),encoding='utf8')
print(len(selected),collections.Counter(x['status'] for x in selected)); print('\n'.join(x['relative_path'] for x in selected[:15]))
