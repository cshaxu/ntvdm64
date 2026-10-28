import csv,io,json,hashlib,subprocess
from pathlib import Path
b=Path('artifacts/mvdm-audit'); prior=json.loads((b/'nine-way/part-1/remaining-guest-scope-evidence.json').read_text(encoding='utf-8-sig'))['rows']; table_data=subprocess.check_output(['git','show','b8aaf0870:artifacts/mvdm-audit/combined-initial-status.csv']); current={x['relative_path']:x for x in csv.DictReader(io.StringIO(table_data.decode('utf-8-sig')))}; roots=[Path('src/mvdm'),Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')];sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest() if p.is_file() else None;rows=[]
for x in prior:
 p=x['path']; status=current.get(p,{}).get('status','missing'); hs=[sha(root/p) for root in roots]; refs=[h for h in hs[1:] if h]; diff=bool(hs[0] and refs and hs[0] not in refs)
 rows.append(dict(shard=x['shard'],path=p,current_status=status,hashes=hs,classification=('remove-from-excluded-candidates' if status!='excluded' else 'keep-partial-mirror-diff' if diff else 'excluded-scope-candidate' if hs[0] and refs else 'identity-unresolved'),explicit_scope=x['raw_evidence']))
head=subprocess.check_output(['git','rev-parse','b8aaf0870'],text=True).strip()
print(json.dumps(dict(head=head,aggregate_sha256=hashlib.sha256(table_data).hexdigest(),source_evidence_sha256=sha(b/'nine-way/part-1/remaining-guest-scope-evidence.json'),rows=rows,limits='Current excluded state and raw file hashes, original missing is not identity; no runtime or source-to-guest proof.'),ensure_ascii=True,indent=2))
