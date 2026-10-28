"""Reconcile the selected original message table, not message behavior."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = 'a0d9d5502', 'wow-message-table-image-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
table = A / 'shard-0/wow-message-table-image-observations.json'
observed = runpy.run_path(str(A / 'shard-0/wow-message-table-image-observer.py'))['inspect']()
assert observed == json.loads(table.read_text(encoding='utf-8-sig'))
assert len(observed['slots']) == 1024 and len(set(observed['hashes'])) == 1
path = observed['source']
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
assert ledger[path]['status'] in ('pending', 'partial') and ledger[path]['shard'] == '0'
proof = dict(relative_path=path, shard='0', sha256=observed['hashes'][0], edition='OpenNT',
    original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='partial',
    role='selected-native-source-partial', evidence_table=table.relative_to(A).as_posix(),
    original_edge='All 1024 original native-to-guest message table relocations match released initial pointers',
    limits='Message execution, reverse mapping, runtime mutation, callbacks, errors and lifecycle remain unproved.',
    original_text_retained_percent='100', estimated_restored_code_percent='',
    restored_estimate_status='unmeasurable: initial table binding does not establish runtime behavior', followup_stage='S14-S15')
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proof), lineterminator='\n')
    writer.writeheader()
    writer.writerow(proof)
H['apply_dispositions']([proof], BASE, 'shard-0/wow-message-table-image-review.md',
    'The original message table is partially reconciled; guest execution and lifecycles remain open.')
result = dict(result='pass', base=BASE, candidate_count=1,
    source_tables=[dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table))],
    host_input_observations=[], scope='Initial native-to-guest message table pointers only')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(result, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, 1)
