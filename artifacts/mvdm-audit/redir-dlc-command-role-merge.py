"""Reconcile selected original DLC command owner with release code evidence."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = 'a4719801d', 'redir-dlc-command-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
table = A / 'shard-0/redir-dlc-command-image-observations.json'
observed = runpy.run_path(str(A / 'shard-0/redir-dlc-command-image-observer.py'))['inspect']()
assert observed == json.loads(table.read_text(encoding='utf-8-sig'))
path = observed['source']
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
assert ledger[path]['status'] in ('pending', 'partial') and len(observed['bindings']) == 17
assert observed['hashes'][0] == observed['hashes'][1] == observed['hashes'][2]
proof = dict(relative_path=path, shard=ledger[path]['shard'], sha256=observed['hashes'][0], edition='OpenNT',
    original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='partial',
    role='selected-native-source-partial', evidence_table=table.relative_to(A).as_posix(),
    original_edge='Seventeen original DLC command bodies and event references bind to released DLL',
    limits='Guest reachability, provider availability, event failures, asynchronous cleanup and teardown remain unproved.',
    original_text_retained_percent='100', estimated_restored_code_percent='',
    restored_estimate_status='unmeasurable: image binding does not establish runtime DLC behavior', followup_stage='S17')
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proof), lineterminator='\n')
    writer.writeheader()
    writer.writerow(proof)
H['apply_dispositions']([proof], BASE, 'shard-0/redir-dlc-command-image-review.md',
    'Original DLC command owner partial; provider and guest behavior remain open.')
result = dict(result='pass', base=BASE, candidate_count=1,
              source_tables=[dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table))],
              host_input_observations=[], scope='Object and event relocation binding, not DLC runtime acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(result, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, 1)
