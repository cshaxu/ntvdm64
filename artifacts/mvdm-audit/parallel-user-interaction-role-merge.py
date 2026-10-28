"""Merge source-verified USER interaction roles; do not claim guest behavior."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = 'c63492da9'
PREFIX = 'parallel-user-interaction-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
contracts = {
    'wow32/waccel.c': ('wow-accelerator', 'Original accelerator bodies and selected call edges bind to the released image', 'Guest dispatch, task identity during cleanup, allocation failure, aliases and teardown remain open.'),
    'wow32/wmsgbm.c': ('wow-button-image', 'Original button conversion bodies and initial dispatch entries bind to the released image', 'Actual guest message round trips, parameters, destruction and reentry remain open.'),
    'wow32/wulang.c': ('wow-language-image', 'Twelve original language thunks, initial table slots and native imports bind to the released image', 'Guest pointer/copyback, DBCS, zero-length, locale, failure and teardown remain open.'),
}
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
frozen = {r['relative_path']: r for r in H['rows'](A / 'frozen-file-reconciliation.csv')}
proofs, tables = [], []
for path, (stem, edge, limits) in contracts.items():
    folder = A / 'shard-1'
    table = folder / (stem + '-observations.json')
    actual = runpy.run_path(str(folder / (stem + '-observer.py')))['inspect']()
    assert actual == json.loads(table.read_text(encoding='utf-8'))
    row, reference = ledger[path], frozen[path]
    assert row['status'] == 'pending'
    assert reference['current_sha256'] == reference['opennt_sha256'] == reference['opennt45_sha256']
    tables.append(dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table)))
    proofs.append(dict(relative_path=path, shard=row['shard'], sha256=reference['current_sha256'],
        edition='OpenNT', original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path),
        prior_status='pending', status='partial', role='selected-native-source-partial',
        evidence_table=table.relative_to(A).as_posix(), original_edge=edge, limits=limits,
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: source and image bindings do not establish complete behavior or lifecycle',
        followup_stage='S12/S15'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, PREFIX + 'review.md',
    'Accelerator, button and language owners are partial; guest behavior and lifecycle remain open.')
observation = dict(result='pass', base=BASE, candidate_count=len(proofs), source_tables=tables,
    host_input_observations=[], scope='Original source and selected image edges only; no guest behavior acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
