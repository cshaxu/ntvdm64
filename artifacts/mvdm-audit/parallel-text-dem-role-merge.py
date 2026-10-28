"""Merge independently verified original tabbed-text and DEM error owners."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = 'd56754f49', 'parallel-text-dem-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
tables = ('shard-1/wow-tabbed-evidence.json', 'shard-3/dem-error-image-observations.json')
scripts = ('shard-1/wow-tabbed-observer.py', 'shard-3/dem-error-image-observer.py')
for name, script in zip(tables, scripts):
    assert runpy.run_path(str(A / script))['inspect']() == json.loads((A / name).read_text(encoding='utf-8'))
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
frozen = {r['relative_path']: r for r in H['rows'](A / 'frozen-file-reconciliation.csv')}
contracts = {
    'wow32/wutext.c': ('Two original tabbed-text dispatch slots and 16 selected call relocations bind to release',
        'Guest invocation, signed tab arrays, mapping/allocation failures, text encoding and lifecycle remain open.', tables[0]),
    'dos/dem/demerror.c': ('Five original bodies, two service slots and four file-open error calls bind to release',
        'Error/CF mapping, hard-error packet identity, retry, nesting and teardown remain open.', tables[1]),
}
proofs = []
for path, (edge, limits, evidence) in contracts.items():
    row, f = ledger[path], frozen[path]
    assert row['status'] == 'pending' and f['current_sha256'] == f['opennt_sha256'] == f['opennt45_sha256']
    proofs.append(dict(relative_path=path, shard=row['shard'], sha256=f['current_sha256'], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending',
        status='partial', role='selected-native-source-partial', evidence_table=evidence,
        original_edge=edge, limits=limits, original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: image wiring does not establish full guest behavior or failure handling',
        followup_stage='S12/S15'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n'); writer.writeheader(); writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'parallel-text-dem-role-review.md',
    'Two original text and DEM error owners are partial; guest behavior and failures remain open.')
observation = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=n, sha256=H['sha'](A / n)) for n in tables], host_input_observations=[],
    scope='Selected original code and linked edges; no guest or failure behavior acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
