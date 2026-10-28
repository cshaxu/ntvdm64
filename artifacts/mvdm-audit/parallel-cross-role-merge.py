"""Merge independently verified WOW USER and BIOS time partial roles."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = 'f7a974aba', 'parallel-cross-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
tables = ('shard-2/parallel-review-fourth-evidence.json', 'shard-3/support-clock-env-observations.json')
scripts = ('shard-2/parallel-review-fourth-observer.py', 'shard-3/support-clock-env-observer.py')
for name, script in zip(tables, scripts):
    assert runpy.run_path(str(A / script))['inspect']() == json.loads((A / name).read_text(encoding='utf-8'))
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
frozen = {r['relative_path']: r for r in H['rows'](A / 'frozen-file-reconciliation.csv')}
contracts = {
    'wow32/wumenu.c': ('Twenty original USER menu ordinals and selected conversion call edges bind to release',
                       'Guest menu interactions, callback reentry, failures and lifetime remain open.', tables[0]),
    'wow32/wucomm.c': ('Seventeen original USER COMM ordinals and task cleanup edge bind to release',
                       'Serial peer behavior, multi-port cleanup, waiters, callbacks and teardown remain open.', tables[0]),
    'softpc.new/base/support/time_day.c': ('BIOS 1Ah dispatch and three original time bodies bind to release',
                       'Clock progression, rollover, RTC/interrupt ordering and reset remain open.', tables[1]),
}
proofs = []
for path, (edge, limits, evidence) in contracts.items():
    row, f = ledger[path], frozen[path]
    assert row['status'] == 'pending' and f['current_sha256'] == f['opennt_sha256'] == f['opennt45_sha256']
    proofs.append(dict(relative_path=path, shard=row['shard'], sha256=f['current_sha256'], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending',
        status='partial', role='selected-native-source-partial', evidence_table=evidence,
        original_edge=edge, limits=limits, original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: initial image binding does not establish full runtime behavior',
        followup_stage='S13/S14/S15'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n'); writer.writeheader(); writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'parallel-cross-role-review.md',
    'Three original WOW USER and BIOS time owners are partial; behavior and lifecycle remain open.')
observation = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=n, sha256=H['sha'](A / n)) for n in tables], host_input_observations=[],
    scope='Initial image dispatch and selected call edges, not behavioral acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs)
)
