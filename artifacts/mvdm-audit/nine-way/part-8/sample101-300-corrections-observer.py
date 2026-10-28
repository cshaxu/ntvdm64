import csv,pathlib
p=pathlib.Path('artifacts/mvdm-audit/nine-way/part-8')
with (p/'existing-classifications-sample101-300.csv').open(encoding='utf-8-sig',newline='') as f:a=list(csv.DictReader(f))
out=[]
for r in a:
 partial=r['classification'] in ('partial','部分复通')
 if partial and r['path'].lower().endswith(('.exe','.com','.dll','.drv')):
  reason='Guest-image partial is not supported by identity alone; recorded limit says actual guest load and feature behavior remain unproved.'
 elif partial:
  reason='Guest source partial is not bound to selected immutable image or the cited execution witness; source hash/diff or generic COMMAND pass does not prove this exact source body executed.'
 else:
  reason='Existing exclusion is explicitly for the historical file or build role while guest behavior, applicable variants or source-to-image lineage remain open; it does not establish overall capability exclusion.'
 out.append({'path':r['path'],'shard':r['shard'],'current':r['classification'],'recommended':'pending','reason':reason,'recorded_evidence':r['evidence'],'recorded_limit':r['limit']})
with (p/'existing-classifications-sample101-300-corrections.csv').open('w',encoding='utf-8',newline='') as f:
 w=csv.DictWriter(f,fieldnames=['path','shard','current','recommended','reason','recorded_evidence','recorded_limit']);w.writeheader();w.writerows(out)
print(len(out),'current status recommendations, no ledger modifications')
