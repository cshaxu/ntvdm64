import csv, hashlib, io, json, subprocess
from collections import Counter
from pathlib import Path
repo=Path(__file__).resolve().parents[3]; ledger_bytes=subprocess.check_output(['git','show','3452abace:artifacts/mvdm-audit/shard-3/file-review.csv'],cwd=repo)
def group(p):
    if p.startswith('softpc.new/base/ccpu386/'): return 'S11 CPU instruction, exception and FPU'
    if p.startswith(('dos/command/','dos/dem/','dpmi32/')): return 'S12 command, DEM and DPMI'
    if p.startswith(('softpc.new/base/bios/','softpc.new/base/system/','softpc.new/base/keymouse/','softpc.new/base/comms/','softpc.new/base/video/','softpc.new/base/cvidc/','softpc.new/base/support/','softpc.new/host/','softpc.new/obj.vdm/')): return 'S12/S13 host, device and lifecycle'
    if p.startswith('vdmredir/'): return 'S12/S17 redirector and network'
    if p.startswith('wow32/'): return 'S14/S15/S16 WOW conversion and lifecycle'
    return 'unmapped'
rows=[]
for r in csv.DictReader(io.StringIO(ledger_bytes.decode('utf-8-sig'))):
    p=r['relative_path']
    if r['review_class']!='partial' or not p.lower().endswith('.c'): continue
    rows.append({'path':p,'group':group(p),'original_sha256':r['opennt_sha256'],'current_sha256':r['current_sha256'],'divergence':r['local_divergence_ids'],'owner':r['source_owner_chain'],'evidence':r['specific_evidence'],'limits':r['specific_limits'],'remaining_proof':r['remaining_proof']})
rows.sort(key=lambda x:x['path'])
print(json.dumps({'ledger_sha256':hashlib.sha256(ledger_bytes).hexdigest(),'count':len(rows),'groups':dict(sorted(Counter(r['group'] for r in rows).items())),'rows':rows},indent=2))
