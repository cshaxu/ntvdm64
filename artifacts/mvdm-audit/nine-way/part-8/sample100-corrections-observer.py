import csv,pathlib
p=pathlib.Path('artifacts/mvdm-audit/nine-way/part-8')
with (p/'existing-classifications-sample100.csv').open(encoding='utf-8-sig',newline='') as f: a=list(csv.DictReader(f))
bins={'bin86/ctl3dv2.dll','bin86/i386/win87em.dll','bin86/lanman.drv','bin86/lzexpand.dll','bin86/netapi.dll','bin86/olecli.dll','bin86/pmspl.dll','bin86/ver.dll','bin86/winhelp.exe'}
rows=[]
for r in a:
 path=r['path']
 if path in bins:
  rows.append({'path':path,'current':r['classification'],'recommended':'pending','reason':'Guest-image partial requires a loaded binary identity tied to actual functional evidence; recorded ZIP/installed identity alone explicitly does not establish loader path or behavior.','evidence':'artifacts/mvdm-audit/nine-way/part-1/bin86-review.md; artifacts/mvdm-audit/nine-way/part-1/bin86-evidence.json'})
 elif path.startswith('dos/v86/cmd/'):
  rows.append({'path':path,'current':r['classification'],'recommended':'pending','reason':'Existing exclusion is explicitly limited to historical file/build role, while guest capability, applicable profile and source-to-delivered-image lineage remain open; it does not establish overall capability exclusion.','evidence':'artifacts/mvdm-audit/nine-way/part-8/existing-classifications-sample100.csv; row evidence and review limit'})
with (p/'existing-classifications-sample100-corrections.csv').open('w',encoding='utf-8',newline='') as f:
 w=csv.DictWriter(f,fieldnames=['path','current','recommended','reason','evidence']);w.writeheader();w.writerows(rows)
print(len(rows), 'proposed corrections',sum(r['path'] in bins for r in rows),'guest images',sum(r['path'].startswith('dos/v86/cmd/') for r in rows),'guest build')
