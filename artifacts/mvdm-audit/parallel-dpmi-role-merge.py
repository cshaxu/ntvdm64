"""Merge the independently replayed DPMI register and stack evidence."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = '1b3513c2e', 'parallel-dpmi-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
evidence = 'shard-3/dpmi-register-stack-image-observations.json'
observed = json.loads((A / evidence).read_text(encoding='utf-8'))
assert runpy.run_path(str(A / 'shard-3/dpmi-register-stack-image-observer.py'))['inspect']() == observed
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
limits = {
    'dpmi32/register.c': 'Register size, SS descriptor width, invalid selectors, nested transitions and error handling remain open.',
    'dpmi32/stack.c': 'Frame bounds/wrap, nested switches, guest return registers/flags, failures and retirement remain open.',
}
proofs = []
for source in observed['sources']:
    path = source['relative_path']; row = ledger[path]
    assert row['status'] == 'pending'
    proofs.append(dict(relative_path=path, shard=row['shard'], sha256=source['sha256'], edition='OpenNT',
        original_path=source['original_path'], prior_status='pending', status='partial',
        role='selected-native-source-partial', evidence_table=evidence,
        original_edge=f"{len(source['functions'])} original bodies and selected DPMI call edges bind to release",
        limits=limits[path], original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: linked bodies do not prove complete guest transitions or lifecycle',
        followup_stage='S11/S12'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n'); writer.writeheader(); writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'shard-3/dpmi-register-stack-role-review.md',
    'Two DPMI owners have original image and call-edge evidence; guest transitions remain open.')
observation = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=evidence, sha256=H['sha'](A / evidence))], host_input_observations=[],
    scope='Selected original code and call edges; no guest transition behavior acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
