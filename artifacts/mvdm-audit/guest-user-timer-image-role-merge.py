"""Reconcile two original guest images without asserting their execution."""
import csv
import json
import runpy
import subprocess
import sys
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = '928abef79', 'guest-user-timer-image-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
timer_path = A / 'shard-2/timer-ne-family-observations.json'
user_path = A / 'shard-3/wow-user-guest-input-observations.json'
timer = runpy.run_path(str(A / 'shard-2/timer-ne-family-observer.py'))['inspect']()
user = json.loads(subprocess.check_output([sys.executable, '-B', str(A / 'shard-3/wow-user-guest-input-observer.py')]))
assert timer == json.loads(timer_path.read_text(encoding='utf-8-sig'))
assert user == json.loads(user_path.read_text(encoding='utf-8-sig'))
t = next(x for x in timer['files'] if x['path'] == 'wow16/timer/TIMER.DRV')
u = next(x for x in user['files'] if x['path'] == 'wow16/user/k/daytona/USER.EXE')
assert len(set(t['hashes'])) == 1 and t['hashes'][0] == timer['timer_dll_sha256']
assert timer['ne_names']['WEP'] == 1 and timer['ne_names']['DRIVERPROC'] == 2
assert u['sha256'] == user['installed_user_exe_sha256']
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
items = [(t['path'], 'OpenNT', t['hashes'][0], timer_path, 'Original timer guest image and NE export ordinals match the archived guest package'),
         (u['path'], 'OpenNT-src-2', u['sha256'], user_path, 'Supplemental original USER guest image matches installed USER.EXE')]
proofs = []
for path, edition, digest, table, edge in items:
    assert ledger[path]['status'] in ('pending', 'partial')
    original = (Path('O:/repos.external/OpenNT/base/mvdm') if edition == 'OpenNT' else
                Path('O:/repos.external/opennt-src-2/nt/private/mvdm')) / path
    proofs.append(dict(relative_path=path, shard=ledger[path]['shard'], sha256=digest, edition=edition,
        original_path=str(original), prior_status='pending', status='partial', role='selected-guest-image-partial',
        evidence_table=table.relative_to(A).as_posix(), original_edge=edge,
        limits='Package/installed identity and export metadata do not prove guest loading, callbacks, errors or task lifecycle.',
        original_text_retained_percent='', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: guest image identity is not runtime behavior', followup_stage='S14-S16'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'guest-user-timer-image-role-review.md',
    'Original USER and TIMER guest images partial; loading, callbacks and lifecycle remain open.')
result = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=p.relative_to(A).as_posix(), sha256=H['sha'](p)) for p in (timer_path, user_path)],
    host_input_observations=[], scope='Guest USER and timer image identity, not runtime acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(result, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
