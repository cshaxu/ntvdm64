"""Reconcile original WOW message heap, memory and shared spool owners."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = '5547d5fad'
PREFIX = 'parallel-wow-memory-spool-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
contracts = {
    'wow32/walloc16.c': ('wow-message-heap', 'Original message heap and callback-stack helpers bind to released WOW32', 'Nested callbacks, serialization, allocation failure, double-free and stack teardown remain open.', 'S14/S15'),
    'wow32/wkmem.c': ('wow-virtual-memory', 'Three original memory thunks and their selected NTVDM imports bind to released WOW32', 'Current worker-image correspondence, actual guest callers, invalid free, commit failures and teardown remain open.', 'S14'),
    'wow32/wspool.c': ('wow-spool-carrier', 'Eleven original spool/carrier functions bind to released WOW32', 'Actual printer success, profile bounds, lock failures, task loss and shared deferred-window carrier lifetimes remain open.', 'S15/S16'),
}
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
frozen = {r['relative_path']: r for r in H['rows'](A / 'frozen-file-reconciliation.csv')}
proofs, tables = [], []
for path, (stem, edge, limits, stage) in contracts.items():
    folder = A / 'shard-1'
    table = folder / (stem + '-observations.json')
    assert runpy.run_path(str(folder / (stem + '-observer.py')))['inspect']() == json.loads(table.read_text(encoding='utf-8'))
    row, source = ledger[path], frozen[path]
    assert row['status'] == 'pending' and row['shard'] == '1'
    assert source['current_sha256'] == source['opennt_sha256'] == source['opennt45_sha256']
    tables.append(dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table)))
    proofs.append(dict(relative_path=path, shard=row['shard'], sha256=source['current_sha256'], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='partial',
        role='selected-native-source-partial', evidence_table=table.relative_to(A).as_posix(), original_edge=edge,
        limits=limits, original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: selected source and image do not prove guest behavior or lifecycle', followup_stage=stage))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, PREFIX + 'review.md',
    'Original WOW heap, memory and spool owners are partial; failures, shared identities and teardown remain open.')
observation = dict(result='pass', base=BASE, candidate_count=len(proofs), source_tables=tables,
    host_input_observations=[], scope='Selected WOW source/image bindings only; no guest behavior acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
