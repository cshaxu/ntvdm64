"""Reconcile nine original bin86 images with the captured guest package."""
import csv
import json
import runpy
import subprocess
import sys
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = '2e3b00992', 'nine-part1-bin86-image-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
table = A / 'nine-way/part-1/bin86-evidence.json'
observed = json.loads(subprocess.check_output([sys.executable, '-B', str(A / 'nine-way/part-1/bin86-observer.py')]))
assert observed == json.loads(table.read_text(encoding='utf-8-sig'))
paths = {'bin86/ctl3dv2.dll', 'bin86/i386/win87em.dll', 'bin86/lanman.drv', 'bin86/lzexpand.dll',
         'bin86/netapi.dll', 'bin86/olecli.dll', 'bin86/pmspl.dll', 'bin86/ver.dll', 'bin86/winhelp.exe'}
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
proofs = []
for item in observed['files']:
    path = item['key']
    if path not in paths:
        continue
    identities = item['identities']
    digest = identities[0]['sha256']
    assert digest and any(z['sha256'] == digest for z in item['zip_entries'])
    assert ledger[path]['status'] in ('pending', 'partial')
    edition, original = ('OpenNT', identities[1]) if identities[1]['sha256'] == digest else ('OpenNT-4.5', identities[2])
    assert original['sha256'] == digest
    installed = item['installed_sha256']
    assert installed is None or installed == digest
    location = 'and installed exact path' if installed else 'but inspected installed exact path was absent'
    proofs.append(dict(relative_path=path, shard=ledger[path]['shard'], sha256=digest.upper(), edition=edition,
        original_path=original['path'], prior_status='pending', status='partial', role='selected-guest-image-partial',
        evidence_table=table.relative_to(A).as_posix(), original_edge=f'Original guest image matches ZIP {location}',
        limits='Actual loader path, source-to-image provenance, API behavior, errors and lifecycle remain unproved.',
        original_text_retained_percent='', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: immutable guest image identity is not behavior', followup_stage='S14-S17'))
assert {p['relative_path'] for p in proofs} == paths
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'nine-way/part-1/bin86-review.md',
    'Nine original guest images match ZIP; deployment and behavior remain distinct obligations.')
result = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table))],
    host_input_observations=[], scope='Guest archive identities only, not runtime acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(result, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
