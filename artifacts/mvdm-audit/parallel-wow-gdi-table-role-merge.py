"""Reconcile original GDI core owner and shared thunk-table initialization."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = 'da9f045d5'
PREFIX = 'parallel-wow-gdi-table-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
contracts = {
    'wow32/wgdi.c': ('shard-0/wow-gdi-core', '119 original GDI table slots and their selected object functions bind to released WOW32', 'Guest dispatch, coordinates, device and escape behavior, callbacks, errors, GDI handles and teardown remain open.', 'S16'),
    'wow32/wowtbl.c': ('shard-1/wow-table-boundary', 'Ten original module separators and initialization body bind to released WOW32', 'Guest initialization, dynamic dispatch and individual API semantics and failures remain open.', 'S11/S14'),
}
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
frozen = {r['relative_path']: r for r in H['rows'](A / 'frozen-file-reconciliation.csv')}
proofs, tables = [], []
for path, (stem, edge, limits, stage) in contracts.items():
    table = A / (stem + '-observations.json')
    observed = json.loads(table.read_text(encoding='utf-8'))
    assert runpy.run_path(str(A / (stem + '-observer.py')))['inspect']() == observed
    row, source = ledger[path], frozen[path]
    assert row['status'] == 'pending' and source['current_sha256'] == source['opennt_sha256'] == source['opennt45_sha256']
    tables.append(dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table)))
    proofs.append(dict(relative_path=path, shard=row['shard'], sha256=source['current_sha256'], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='partial',
        role='selected-native-source-partial', evidence_table=table.relative_to(A).as_posix(), original_edge=edge,
        limits=limits, original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: table and object bindings do not establish guest behavior or lifecycle', followup_stage=stage))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, PREFIX + 'review.md',
    'Original GDI and WOW table owners are partial; guest execution and lifecycle remain open.')
observation = dict(result='pass', base=BASE, candidate_count=len(proofs), source_tables=tables,
    host_input_observations=[], scope='Original source and initial table/image bindings only; no guest behavior acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
