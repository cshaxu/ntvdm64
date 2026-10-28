import csv,hashlib,json,pathlib
p=pathlib.Path('artifacts/mvdm-audit/nine-way/part-8'); root=p.parents[1]
inputs=[p/'uncovered-sample-r3-observer.py',p/'existing-classifications-sample100.csv',p/'existing-classifications-sample101-300.csv',p/'existing-classifications-uncovered-r2.csv',root/'nine-way/part-1/other-guest-excluded-scope-evidence.json',p/'existing-classifications-uncovered-r3.csv']
meta={'note':'Additional frozen input identities; original baseline omits the r2 exclusion label, although the frozen generator uses it. Original snapshot unchanged.', 'inputs':[{'path':str(x),'sha256':hashlib.sha256(x.read_bytes()).hexdigest().upper()} for x in inputs]}
(p/'uncovered-r3-freeze-supplement.json').write_text(json.dumps(meta,indent=2),encoding='utf-8')
rows=list(csv.DictReader((p/'existing-classifications-uncovered-r3.csv').open(encoding='utf-8-sig')))
rec={r['relative_path'].lower():r for r in csv.DictReader((root/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
out=[]
for x in rows:
 r=rec.get(x['path'].lower(),{}); orig=r.get('opennt_sha256',''); other=r.get('opennt45_sha256',''); cur=r.get('current_sha256',''); identity=r.get('current_identity','unknown')
 role='retain existing; verify scope'
 if x['classification'] in ('excluded','æ•´ä½“æŽ’é™¤') and pathlib.PurePosixPath(x['path']).suffix.lower() in ('.asm','.c','.h','.inc','.cl3','.cl4','.cl5','.skl'):
  role='pending candidate: source/data role alone does not establish capability exclusion'
 if orig and cur and cur!=orig and (not other or cur!=other): role='mirror diff: must remain partial; do not downgrade'
 out.append(dict(path=x['path'],classification=x['classification'],current_identity=identity,original_sha256=orig,original45_sha256=other,current_sha256=cur,recommendation=role,evidence=x['evidence'],limit=x['limit']))
with (p/'uncovered-r3-file-review.csv').open('w',encoding='utf-8',newline='') as f:
 w=csv.DictWriter(f,fieldnames=list(out[0]));w.writeheader();w.writerows(out)
print('rows',len(out));from collections import Counter;print(Counter(x['recommendation'] for x in out))
