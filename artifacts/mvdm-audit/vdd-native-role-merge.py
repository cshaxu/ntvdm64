"""Merge the bounded VDD source/private-declaration audit, not a runtime fix."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = '6f7ca8722', 'vdd-native-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
observed = runpy.run_path(str(A / 'vdd-image-observer.py'))['inspect']()
assert observed == json.loads((A / 'vdd-image-observations.json').read_text())
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
proofs = []
for row in observed['sources']:
    header = row['relative_path'].endswith('.h')
    chain = ('Original private MEM_HOOK_DATA/VDDPROC and LPT status declarations -> selected nt_vdd/nt_msscs owners; real x86 fixture ABI checks' if header else
             'Original VDD APIs -> adapter ownership/IOS port routing, IRQ reservations and ICA-locked DMA state; nine bodies bound to released image, actual cached objects used by contract fixture')
    limits = ('Declarations are not a memory-hook lifecycle implementation; guest faults, nested registration, unload, reset and rollback belong to their original consumers in S13. ' if header else '') + (
        'Strict native-object probes prove stale I/O callbacks for all seven uninstall slots and out-of-bounds DMA channel 8 access/write in all three DMA APIs. S13 must repair both with controls, multi-owner lifetime and product regressions. Real VDD DLL load/unload, concurrent ownership, failure rollback and port-endpoint range/overflow rules are not accepted by normal controls.')
    proofs.append(dict(relative_path=row['relative_path'], shard=ledger[row['relative_path']]['shard'],
        sha256=row['sha256'], edition='OpenNT', original_path=row['original_path'], prior_status='pending', status='partial',
        role='selected-native-source-partial', evidence_table='vdd-image-observations.json', original_edge=chain,
        limits=limits, original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: 100% primary source bytes retained; nine linked bodies, 305 normal checks and two strict counterexamples cannot measure full VDD guest/lifetime recovery',
        followup_stage='S13'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'vdd-native-role-review.md',
    'Two original VDD implementation/private-declaration owners partial; nine image-bound bodies and 305 normal assertions. Two actual-object lifetime/boundary defects retained for S13. No runtime or guest change.')
observation = dict(result='pass', base=BASE, candidate_count=2,
    source_tables=[dict(path='vdd-image-observations.json', sha256=H['sha'](A / 'vdd-image-observations.json'))],
    host_input_observations=[], scope='Original VDD source/private declaration and controlled object contracts; no complete guest lifecycle or new release acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, 2)
