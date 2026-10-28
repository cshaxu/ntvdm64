"""Classify two historical build inputs without excluding DOS messages."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = '9825a2b4c', 'dos-message-input-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
table = A / 'shard-2/dos-message-input-observations.json'
observed = runpy.run_path(str(A / 'shard-2/dos-message-input-observer.py'))['inspect']()
assert observed == json.loads(table.read_text(encoding='utf-8-sig'))
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
wanted = {'dos/v86/messages/loadmsg.msg', 'dos/v86/messages/usa-ms.idx'}
proofs = []
for source in observed['sources']:
    path = source['relative_path']
    if path not in wanted:
        continue
    row = ledger[path]
    assert row['shard'] == '2' and row['status'] in ('pending', 'excluded')
    edge = ('LOADFIX assembler and recipe consume original message input' if path.endswith('loadmsg.msg')
            else 'Historical BUILDMSG opens a generated country message index')
    proofs.append(dict(relative_path=path, shard=row['shard'], sha256=source['sha256'], edition='OpenNT',
        original_path=source['original_path'], prior_status='pending', status='excluded',
        role='historical-guest-build-input-only', evidence_table=table.relative_to(A).as_posix(),
        original_edge=edge,
        limits='File-only historical build role; guest messages, locale support and image provenance remain open.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='not-applicable: guest build input, not runtime code coverage', followup_stage='S12'))
assert {p['relative_path'] for p in proofs} == wanted
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'shard-2/dos-message-input-review.md',
    'Two historical DOS message build inputs excluded by file role; locale and runtime behavior remain open.')
result = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table))],
    host_input_observations=[], scope='Guest build input roles, not DOS locale behavior')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(result, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
