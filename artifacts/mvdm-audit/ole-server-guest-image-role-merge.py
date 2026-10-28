"""Reconcile the original retail OLE server image with the guest archive."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = '54624d313', 'ole-server-guest-image-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
table = A / 'shard-0/ole-server-image-observations.json'
observed = runpy.run_path(str(A / 'shard-0/ole-server-image-observer.py'))['inspect']()
assert observed == json.loads(table.read_text(encoding='utf-8-sig'))
source = next(x for x in observed['sources'] if x['path'] == 'wow16/ole/server/retail/OLESVR.DLL')
assert len(set(source['hashes'])) == 1 and source['hashes'][0] == observed['packaged_sha256']
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
path = source['path']
assert ledger[path]['shard'] == '0' and ledger[path]['status'] in ('pending', 'partial')
proof = dict(relative_path=path, shard='0', sha256=source['hashes'][0], edition='OpenNT',
    original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='partial',
    role='selected-guest-image-partial', evidence_table=table.relative_to(A).as_posix(),
    original_edge='Original retail OLE server guest image matches the archived guest package',
    limits='Exact installation path was absent; guest loading, source-to-image provenance, OLE behavior and lifetime remain unproved.',
    original_text_retained_percent='', estimated_restored_code_percent='',
    restored_estimate_status='unmeasurable: guest image identity is not runtime behavior', followup_stage='S14-S16')
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proof), lineterminator='\n')
    writer.writeheader()
    writer.writerow(proof)
H['apply_dispositions']([proof], BASE, 'shard-0/ole-server-image-review.md',
    'Original OLE server retail guest image matches archive; actual loading and runtime remain open.')
result = dict(result='pass', base=BASE, candidate_count=1,
    source_tables=[dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table))],
    host_input_observations=[], scope='Archived guest image identity only')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(result, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, 1)
