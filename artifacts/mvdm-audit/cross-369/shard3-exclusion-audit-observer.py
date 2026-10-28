import csv, hashlib, io, json, subprocess
from collections import Counter
from pathlib import Path
repo=Path(__file__).resolve().parents[3]
path='artifacts/mvdm-audit/shard-3/file-review.csv'
revision='c209d364771979e0ea0d5bcc30f51cb05e12871d'
content=subprocess.check_output(['git','-C',str(repo),'show',revision+':'+path])
rows=list(csv.DictReader(io.StringIO(content.decode('utf-8-sig'),newline='')))
excluded=[r for r in rows if r['review_class']=='excluded']
with_mirrors=[r for r in excluded if r['current_sha256'] and (r['opennt_sha256'] or r['opennt45_sha256'])]
diff=[r['relative_path'] for r in with_mirrors if r['current_sha256'] not in {r['opennt_sha256'],r['opennt45_sha256']}]
keys=['softpc.new/base/ccpu386/c_getset.c','softpc.new/base/ccpu386/ccpupig.c','softpc.new/base/ccpu386/nop.c','wow32/wthman.c']
fields=['relative_path','review_class','review_basis','specific_limits','current_identity','opennt_sha256','opennt45_sha256','current_sha256']
print(json.dumps({'ledger_sha256':hashlib.sha256(content).hexdigest(),'excluded_count':len(excluded),'identity_counts':dict(sorted(Counter(r['current_identity'] for r in excluded).items())),'excluded_with_recorded_mirrors_count':len(with_mirrors),'different_from_both_mirrors':sorted(diff),'sample_rows':[{k:r[k] for k in fields} for r in rows if r['relative_path'] in keys]},indent=2))
