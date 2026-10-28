"""Merge independently verified original WOW heap and C-VID roles."""
import csv
import json
import runpy
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = 'fbe870930', 'parallel-heap-video-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
tables = ('shard-3/wow-heap-image-observations.json', 'shard-0/sinit011-image-observations.json')
subprocess.run(['python', '-B', str(A / 'shard-3/wow-heap-durable-check.py')], check=True)
assert runpy.run_path(str(A / 'shard-0/sinit011-image-observer.py'))['inspect']() == json.loads((A / tables[1]).read_text(encoding='utf-8'))
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
frozen = {r['relative_path']: r for r in H['rows'](A / 'frozen-file-reconciliation.csv')}
contracts = {
    'wow32/wheap.c': ('Three original heap bodies and five selected calls bind to released WOW32',
        'Cross-task serialization, allocation failures, explicit unload and late callbacks remain open.', tables[0]),
    'wow32/wheap.h': ('Original heap declarations are selected by actual WOW compiler contexts',
        'ABI, concurrent ownership and all consumer lifetimes remain open.', tables[0]),
    'softpc.new/base/cvidc/sinit011.c': ('Twenty-one original C-VID accessors bind to released NTVDM',
        'Actual mode-specific access, shared scratch, reentry and retirement remain open.', tables[1]),
}
proofs = []
for path, (edge, limits, evidence) in contracts.items():
    row, f = ledger[path], frozen[path]
    assert row['status'] == 'pending' and f['current_sha256'] == f['opennt_sha256'] == f['opennt45_sha256']
    proofs.append(dict(relative_path=path, shard=row['shard'], sha256=f['current_sha256'], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending',
        status='partial', role='selected-native-source-partial', evidence_table=evidence,
        original_edge=edge, limits=limits, original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: bound code or declarations do not establish complete runtime behavior',
        followup_stage='S11/S13/S14'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n'); writer.writeheader(); writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'parallel-heap-video-role-review.md',
    'Original WOW heap and C-VID accessor roles are partial; behavior and lifecycle remain open.')
observation = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=n, sha256=H['sha'](A / n)) for n in tables], host_input_observations=[],
    scope='Selected original code/declarations and bounded heap test; no complete lifetime acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
