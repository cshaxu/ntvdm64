import csv, hashlib, io, json, subprocess
from collections import Counter
from pathlib import Path
repo=Path(__file__).resolve().parents[3]; ledger_bytes=subprocess.check_output(['git','show','3452abace:artifacts/mvdm-audit/shard-3/file-review.csv'],cwd=repo)
def group(p):
    if p.startswith('softpc.new/base/ccpu386/'): return 'CPU declarations and macros'
    if p.startswith('softpc.new/base/cvidc/'): return 'generated C-VID CPU and FPU interfaces'
    if p.startswith('softpc.new/base/inc/'): return 'device, memory and BIOS interfaces'
    if p.startswith('softpc.new/host/inc/'): return 'host adapter interface'
    if p.startswith('wow32/'): return 'WOW dispatch and conversion interface'
    if p.startswith('inc/'): return 'guest-host ABI or service interface'
    if p.startswith('dpmi32/'): return 'DPMI translation interface'
    return 'unmapped'
rows=[]
for r in csv.DictReader(io.StringIO(ledger_bytes.decode('utf-8-sig'))):
    p=r['relative_path']
    if r['review_class']!='partial' or not p.lower().endswith('.h'):continue
    rows.append({'path':p,'group':group(p),'original_sha256':r['opennt_sha256'],'current_sha256':r['current_sha256'],'divergence':r['local_divergence_ids'],'owner':r['source_owner_chain'],'evidence':r['specific_evidence'],'limits':r['specific_limits'],'remaining_proof':r['remaining_proof']})
rows.sort(key=lambda x:x['path'])
print(json.dumps({'ledger_sha256':hashlib.sha256(ledger_bytes).hexdigest(),'count':len(rows),'groups':dict(sorted(Counter(x['group'] for x in rows).items())),'rows':rows},indent=2))
