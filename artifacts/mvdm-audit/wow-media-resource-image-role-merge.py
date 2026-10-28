"""Record selected original multimedia and resource bodies without claiming behavior."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = 'bf92afb7d', 'wow-media-resource-image-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
table = A / 'shard-0/wow-media-resource-image-observations.json'
observed = runpy.run_path(str(A / 'shard-0/wow-media-resource-image-observer.py'))['inspect']()
assert observed == json.loads(table.read_text(encoding='utf-8-sig'))
assert [(x['path'], len(x['bindings'])) for x in observed['sources']] == [('wow32/wmmedia.c', 4), ('wow32/wres32.c', 6)]
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
proofs = []
for source in observed['sources']:
    path = source['path']
    assert len(set(source['hashes'])) == 1 and ledger[path]['shard'] == '0'
    assert ledger[path]['status'] in ('pending', 'partial')
    proofs.append(dict(relative_path=path, shard='0', sha256=source['hashes'][0], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='partial',
        role='selected-native-source-partial', evidence_table=table.relative_to(A).as_posix(),
        original_edge=f"{len(source['bindings'])} original function bodies bind to released WOW32 executable code",
        limits='Real guest dispatch, host and guest identities, callbacks, failure and lifecycle remain unproved.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: body binding is not runtime behavior', followup_stage='S14-S16'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'shard-0/wow-media-resource-image-review.md',
    'Multimedia and resource thunk owners partial; guest dispatch and lifecycle remain open.')
result = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table))],
    host_input_observations=[], scope='WOW function body binding only')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(result, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
