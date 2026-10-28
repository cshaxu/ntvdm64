"""Reconcile original WOW palette bindings without asserting guest behavior."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = '867f8291d'
PREFIX = 'shard0-wow-palette-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
table = A / 'shard-0/wow-palette-image-observations.json'
observed = json.loads(table.read_text(encoding='utf-8'))
assert runpy.run_path(str(A / 'shard-0/wow-palette-image-observer.py'))['inspect']() == observed
assert len(observed['gdi']['owners'][0]['entries']) == 10 and len(observed['user_entries']) == 2
path = 'wow32/wgpal.c'
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
frozen = {r['relative_path']: r for r in H['rows'](A / 'frozen-file-reconciliation.csv')}
row, source = ledger[path], frozen[path]
assert row['status'] == 'pending' and row['shard'] == '0'
assert source['current_sha256'] == source['opennt_sha256'] == source['opennt45_sha256']
proof = dict(relative_path=path, shard=row['shard'], sha256=source['current_sha256'], edition='OpenNT',
    original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending',
    status='partial', role='selected-native-source-partial', evidence_table=table.relative_to(A).as_posix(),
    original_edge='Ten original GDI and two USER palette thunks bind to published WOW32 initial table',
    limits='Guest dispatch, handle identity, copyback on short or failed host results, device changes, reentry and teardown remain open.',
    original_text_retained_percent='100', estimated_restored_code_percent='',
    restored_estimate_status='unmeasurable: initial image bindings do not establish guest behavior or lifecycle', followup_stage='S16')
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proof), lineterminator='\n')
    writer.writeheader()
    writer.writerow(proof)
H['apply_dispositions']([proof], BASE, 'shard-0/wow-palette-image-review.md',
    'Palette owner is partial; guest behavior, host short-result copyback and lifecycle remain open.')
observation = dict(result='pass', base=BASE, candidate_count=1,
    source_tables=[dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table))],
    host_input_observations=[], scope='Initial palette source and table bindings only; no guest behavior acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, 1)
