"""Reconcile original host service headers with guest constant evidence."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit'); BASE = '9f50825e1'; PREFIX = 'guest-service-header-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
table = A / 'shard-1/guest-service-constants-observations.json'; observed = json.loads(table.read_text(encoding='utf-8'))
assert runpy.run_path(str(A / 'shard-1/guest-service-constants-observer.py'))['inspect']() == observed
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
frozen = {r['relative_path']: r for r in H['rows'](A / 'frozen-file-reconciliation.csv')}
selected = {'DOSSVC': 'inc/dossvc.h', 'DPMI': 'inc/dpmi.h', 'XMSSVC': 'inc/xmssvc.h'}
proofs = []
for item in observed['pairs']:
    if item['name'] not in selected: continue
    path = selected[item['name']]; row, source = ledger[path], frozen[path]
    assert row['status'] == 'pending' and item['c_valid_dependencies']
    assert item['c_sha256'] == source['current_sha256'] == source['opennt_sha256'] == source['opennt45_sha256']
    proofs.append(dict(relative_path=path, shard=row['shard'], sha256=item['c_sha256'], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='partial',
        role='selected-native-source-partial', evidence_table=table.relative_to(A).as_posix(),
        original_edge='Original C header has normalized valid compiler dependencies and matching simple guest literals',
        limits='Guest image, expression-valued definitions, selector dispatch, errors and service lifecycle remain unproved.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: matching literals and dependencies do not establish service behavior', followup_stage='S11-S13'))
assert len(proofs) == 3 and {x['relative_path'] for x in proofs} == set(selected.values())
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n'); writer.writeheader(); writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, PREFIX + 'review.md',
    'Three selected service declaration headers remain partial; guest service and lifecycle behavior remains in scope.')
observation = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table))], host_input_observations=[],
    scope='Selected original C header and simple cross-language constants; no guest runtime acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
