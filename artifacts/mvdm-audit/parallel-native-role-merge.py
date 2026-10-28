"""Merge independently replayed original-source partial candidates."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = '69fa3fd4d', 'parallel-native-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
native = json.loads((A / 'shard-2/parallel-review-evidence.json').read_text(encoding='utf-8'))
sinit = json.loads((A / 'shard-3/sinit012-image-observations.json').read_text(encoding='utf-8'))
assert runpy.run_path(str(A / 'shard-2/parallel-review-observer.py'))['inspect']() == native
assert runpy.run_path(str(A / 'shard-3/sinit012-image-observer.py'))['inspect']() == sinit
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
frozen = {r['relative_path']: r for r in H['rows'](A / 'frozen-file-reconciliation.csv')}
contracts = {
    'dos/command/cmddisp.c': ('Original command dispatcher bound to image', 'Guest BOP reachability, bounds, return and lifecycle remain open.'),
    'dos/dem/demdasd.c': ('Original absolute disk read and write bodies bound to image', 'Media identity, geometry, errors and cleanup remain open.'),
    'dos/dem/demioctl.c': ('Original IOCTL dispatcher bound to image', 'Direct BOP reachability, subfunction bounds, errors and lifecycle remain open.'),
    'dos/dem/demlock.c': ('Original file-range lock body bound to image', 'Cross-process locking, errors and handle retirement remain open.'),
    'softpc.new/base/cvidc/sinit012.c': ('Original generated C-VID object has 33 image-bound bodies', 'Mode-specific execution, backing state, concurrency and retirement remain open.'),
}
assert set(contracts) == set(native['native_bindings']) | {sinit['relative_path']}
proofs = []
for path, (edge, limits) in contracts.items():
    record, original = ledger[path], frozen[path]
    assert record['status'] == 'pending'
    assert original['current_sha256'] == original['opennt_sha256'] == original['opennt45_sha256']
    proofs.append(dict(relative_path=path, shard=record['shard'], sha256=original['current_sha256'],
        edition='OpenNT', original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path),
        prior_status='pending', status='partial', role='selected-native-source-partial',
        evidence_table='shard-3/sinit012-image-observations.json' if path == sinit['relative_path'] else 'shard-2/parallel-review-evidence.json',
        original_edge=edge, limits=limits, original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: bound code does not establish complete execution or lifetime behavior',
        followup_stage='S11/S12/S13'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'parallel-native-role-review.md',
    'Five selected original DOS and C-VID owners are partial; behavior and lifecycle remain open.')
observation = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=name, sha256=H['sha'](A / name)) for name in
        ('shard-2/parallel-review-evidence.json', 'shard-3/sinit012-image-observations.json')],
    host_input_observations=[], scope='Original source and image binding only, not behavioral acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
