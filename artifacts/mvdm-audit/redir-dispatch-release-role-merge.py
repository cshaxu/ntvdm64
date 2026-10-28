"""Reconcile original redirector dispatcher with released table evidence."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = '6085452e7', 'redir-dispatch-release-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
table = A / 'shard-0/redir-dispatch-release-observations.json'
observed = runpy.run_path(str(A / 'shard-0/redir-dispatch-release-observer.py'))['inspect']()
assert observed == json.loads(table.read_text(encoding='utf-8-sig'))
path = observed['source']
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
assert ledger[path]['status'] in ('pending', 'partial') and len(observed['entries']) == 50
assert observed['hashes'][0] == observed['hashes'][1] == observed['hashes'][2]
proof = dict(relative_path=path, shard=ledger[path]['shard'], sha256=observed['hashes'][0], edition='OpenNT',
    original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='partial',
    role='selected-native-source-partial', evidence_table=table.relative_to(A).as_posix(),
    original_edge='Original dispatcher and all 50 service-table targets bound to released DLL',
    limits='Guest reachability, external providers, asynchronous completion, errors and teardown remain unproved.',
    original_text_retained_percent='100', estimated_restored_code_percent='',
    restored_estimate_status='unmeasurable: static dispatch binding is not guest behavior coverage', followup_stage='S17')
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proof), lineterminator='\n')
    writer.writeheader()
    writer.writerow(proof)
H['apply_dispositions']([proof], BASE, 'shard-0/redir-dispatch-release-review.md',
    'Original redirector dispatch role partial; guest and network behavior remain open.')
result = dict(result='pass', base=BASE, candidate_count=1,
              source_tables=[dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table))],
              host_input_observations=[], scope='Selected dispatch and table targets, not network behavior acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(result, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, 1)
