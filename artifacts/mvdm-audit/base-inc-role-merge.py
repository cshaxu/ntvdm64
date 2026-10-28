"""Classify five original base headers selected in production preprocessing."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = '6c0df5292', 'base-inc-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
observed = json.loads((A / 'base-inc-selection-observations.json').read_text())
assert runpy.run_path(str(A / 'base-inc-selection-observer.py'))['inspect']() == observed
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
limits = {
    'bios.h': 'Prove BIOS state layout, callers, reset and guest-visible behavior across consumers.',
    'config.h': 'Prove configuration values, consumer ABI, mutation, error handling and lifetimes.',
    'cpu4.h': 'Prove CPU and hardware-interrupt entry ordering, ABI and failure behavior.',
    'timer.h': 'Prove timer/EOI layout, ownership, reset, concurrency and guest timing.',
    'xt.h': 'Prove shared CPU types and effective-address ABI, callers, fault ordering and lifetime.',
}
proofs = []
for row in observed['sources']:
    name = row['relative_path']
    contexts = ', '.join(s['translation_unit'] for s in row['selections'])
    proofs.append(dict(relative_path=name, shard=ledger[name]['shard'], sha256=row['sha256'],
        edition='OpenNT', original_path=row['original_path'], prior_status='pending', status='partial',
        role='selected-native-source-partial', evidence_table='base-inc-selection-observations.json',
        original_edge='Byte-identical original declarations emitted by selected production preprocessing in ' + contexts,
        limits=limits[Path(name).name], original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: selected declarations do not establish runtime behavior or ABI coverage',
        followup_stage='S11/S12/S13'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'base-inc-role-review.md',
    'Five byte-identical original common headers selected by actual CCPU preprocessing are partial; other contexts and behavioral contracts remain open.')
observation = dict(result='pass', base=BASE, candidate_count=5,
    source_tables=[dict(path='base-inc-selection-observations.json', sha256=H['sha'](A / 'base-inc-selection-observations.json'))],
    host_input_observations=[], scope='Five selected declaration roles, not full ABI or product behavior')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, 5)
