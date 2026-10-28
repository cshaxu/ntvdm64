"""Reconcile delivered guest driver and sampled historical executable stub."""
import csv
import json
import runpy
import subprocess
import sys
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = 'd84daa8be', 'win16-guest-image-payload-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
stub_path = A / 'shard-2/win16-kernel-stub-owner-observations.json'
network_path = A / 'shard-3/wfwnet-guest-input-observations.json'
stub = runpy.run_path(str(A / 'shard-2/win16-kernel-stub-owner-observer.py'))['inspect']()
network = json.loads(subprocess.check_output([sys.executable, '-B', str(A / 'shard-3/wfwnet-guest-input-observer.py')]))
assert stub == json.loads(stub_path.read_text(encoding='utf-8-sig'))
assert network == json.loads(network_path.read_text(encoding='utf-8-sig'))
assert stub['stub_payload_compared_bytes'] == 546
assert stub['release_images'][0]['mz_payload_matches_stub'] and stub['release_images'][1]['mz_payload_matches_stub']
row = next(x for x in network['files'] if x['path'] == 'wow16/wfwnet/WFWNET.DRV')
assert row['sha256'] == row['opennt_sha256'] == row['opennt45_sha256'] == network['installed_sha256']
stub_source = next(x for x in stub['files'] if x['path'] == 'wow16/bin/winstub.exe')
assert stub_source['hashes'][0] == stub_source['hashes'][2] and not stub_source['hashes'][1]
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
items = [('wow16/bin/winstub.exe', 'OpenNT-4.5', stub_source['hashes'][0], 'shard-2/win16-kernel-stub-owner-observations.json',
          '546-byte stub payload matches installed WRITE and COMM images; standalone guest execution remains unproved'),
         ('wow16/wfwnet/WFWNET.DRV', 'OpenNT', row['sha256'], 'shard-3/wfwnet-guest-input-observations.json',
          'Original network driver matches installed guest image; selected runtime and providers remain unproved')]
proofs = []
for path, edition, digest, table, edge in items:
    shard = ledger[path]['shard']
    assert ledger[path]['status'] in ('pending', 'partial')
    original = (Path('O:/repos.external/OpenNT/base/mvdm') if edition == 'OpenNT' else
                Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')) / path
    proofs.append(dict(relative_path=path, shard=shard, sha256=digest, edition=edition, original_path=str(original),
        prior_status='pending', status='partial', role='selected-guest-image-partial', evidence_table=table,
        original_edge=edge, limits='Payload/image identity is not guest execution, source-to-image lineage, errors or lifecycle proof.',
        original_text_retained_percent='', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: immutable guest bytes or partial payload, not code behavior coverage', followup_stage='S14-S17'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'win16-guest-image-payload-role-review.md',
    'Two guest image/payload identities partial; execution and task lifecycle remain open.')
result = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=p.relative_to(A).as_posix(), sha256=H['sha'](p)) for p in (stub_path, network_path)],
    host_input_observations=[], scope='Two immutable guest image/payload identities, not guest acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(result, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
