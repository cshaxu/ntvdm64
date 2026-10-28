import csv,io,json,subprocess
from pathlib import Path
base=Path('artifacts/mvdm-audit')
def snapshot(name):
 data=subprocess.check_output(['git','show','7a0a22f32:'+str(base/name).replace('\\','/')])
 return csv.DictReader(io.StringIO(data.decode('utf-8-sig')))
combined={x['relative_path']:x for x in snapshot('combined-initial-status.csv') if x['shard']=='1' and x['status']=='excluded'}
rows=[]
for x in snapshot('shard-1/file-audit.csv'):
 p=x['relative_path'];b=x['classification_basis']
 if p not in combined or not b.startswith('Historical immutable guest build source/resource only; excluded as current host implementation input, not as guest functionality.'):continue
 rows.append(dict(path=p,existing='excluded',recommended='pending-overall-role-scoped-exclusion-retained',basis=b,evidence=x['evidence'],unresolved=x['next_required_evidence']))
print(json.dumps(dict(count=len(rows),rows=rows,limits='Exact selection uses shard-1 snapshot and its explicit scoped host-only basis; source-to-image and guest capability remain unresolved. Historical object/list/recipe roles not mechanically reclassified.'),ensure_ascii=True,indent=2))
