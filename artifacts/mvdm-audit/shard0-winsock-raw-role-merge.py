"""Reconcile the original synchronous Winsock source owner."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = '12bd70ab1'
PREFIX = 'shard0-winsock-raw-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
table = A / 'shard-0/wow-winsock-raw-observations.json'
observed = json.loads(table.read_text(encoding='utf-8'))
assert runpy.run_path(str(A / 'shard-0/wow-winsock-raw-observer.py'))['inspect']() == observed
assert len(observed['entries']) == 23
path = 'wow32/wsraw.c'
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
frozen = {r['relative_path']: r for r in H['rows'](A / 'frozen-file-reconciliation.csv')}
row, source = ledger[path], frozen[path]
assert row['status'] == 'pending' and row['shard'] == '0'
assert source['current_sha256'] == source['opennt_sha256'] == source['opennt45_sha256'] == observed['source_sha256']
proof = dict(relative_path=path, shard=row['shard'], sha256=source['current_sha256'], edition='OpenNT',
    original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='partial',
    role='selected-native-source-partial', evidence_table=table.relative_to(A).as_posix(),
    original_edge='Original synchronous Winsock ordinal slots 1–23 bind to released WOW32 object bodies',
    limits='External peers, all guest APIs, task identities, blocking callbacks, failures and cleanup remain open.',
    original_text_retained_percent='100', estimated_restored_code_percent='',
    restored_estimate_status='unmeasurable: selected table bindings and a limited prior loopback do not establish full behavior', followup_stage='S16')
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proof), lineterminator='\n')
    writer.writeheader()
    writer.writerow(proof)
H['apply_dispositions']([proof], BASE, 'shard-0/wow-winsock-raw-review.md',
    'Original Winsock synchronous owner is partial; guest operations, provider failures and lifecycle remain open.')
observation = dict(result='pass', base=BASE, candidate_count=1,
    source_tables=[dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table))],
    host_input_observations=[], scope='Original source and initial 23 Winsock bindings only; no broad network acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, 1)
