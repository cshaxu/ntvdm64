"""Reconcile original WOW shell, hook and subclass owners, not their behavior."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = 'e4f7690ba', 'wow-shell-hook-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
table = A / 'shard-0/wow-shell-hook-image-observations.json'
observed = runpy.run_path(str(A / 'shard-0/wow-shell-hook-image-observer.py'))['inspect']()
assert observed == json.loads(table.read_text(encoding='utf-8-sig'))
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
assert {r['path'] for r in observed['sources']} == {'wow32/wshell.c', 'wow32/wsubcls.c', 'wow32/wuhook.c'}
assert [len(r['bindings']) for r in observed['sources']] == [22, 2, 5]
proofs = []
for source in observed['sources']:
    path = source['path']
    assert ledger[path]['status'] in ('pending', 'partial')
    assert len(set(source['hashes'])) == 1
    proofs.append(dict(relative_path=path, shard=ledger[path]['shard'], sha256=source['hashes'][0],
        edition='OpenNT', original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path),
        prior_status='pending', status='partial', role='selected-native-source-partial',
        evidence_table=table.relative_to(A).as_posix(),
        original_edge=f"{len(source['bindings'])} original function bodies bind to released WOW32 code",
        limits='Guest dispatch, callback identity and nesting, failure and task/window retirement remain unproved.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: body binding does not establish guest behavior', followup_stage='S14'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'shard-0/wow-shell-hook-image-review.md',
    'Selected WOW shell, hook and subclass owners partial; guest and lifecycle behavior remain open.')
result = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table))],
    host_input_observations=[], scope='WOW function body binding, not guest behavior acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(result, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
