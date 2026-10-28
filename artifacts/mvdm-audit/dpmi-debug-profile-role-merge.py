"""Reconcile current release DPMI debug-only body without excluding DPMI."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = '4780626d0', 'dpmi-debug-profile-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
table = A / 'shard-2/dpmi-debug-selected-image-observations.json'
observed = runpy.run_path(str(A / 'shard-2/dpmi-debug-selected-image-observer.py'))['inspect']()
assert observed == json.loads(table.read_text(encoding='utf-8-sig'))
path = observed['source']
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
assert ledger[path]['status'] in ('pending', 'excluded') and observed['hashes'][0] == observed['hashes'][1] == observed['hashes'][2]
proof = dict(relative_path=path, shard=ledger[path]['shard'], sha256=observed['hashes'][0], edition='OpenNT',
    original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='excluded',
    role='selected-debug-only-source-file', evidence_table=table.relative_to(A).as_posix(),
    original_edge='Compiled and archived object has no code; original body gated by DBG',
    limits='Current release profile only; original DPMI and other debug profiles remain in scope.',
    original_text_retained_percent='100', estimated_restored_code_percent='',
    restored_estimate_status='not-applicable: current release debug-only body; DPMI capability retained', followup_stage='S11-S12')
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proof), lineterminator='\n')
    writer.writeheader()
    writer.writerow(proof)
H['apply_dispositions']([proof], BASE, 'shard-2/dpmi-debug-selected-image-review.md',
    'One current-release debug-only file role excluded; DPMI behavior remains in scope.')
result = dict(result='pass', base=BASE, candidate_count=1,
              source_tables=[dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table))],
              host_input_observations=[], scope='Debug-only file body, not DPMI capability exclusion')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(result, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, 1)
